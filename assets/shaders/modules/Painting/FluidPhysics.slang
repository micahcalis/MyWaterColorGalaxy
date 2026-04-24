#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Physics.Flux;

module FluidPhysics;

namespace Painting
{
    public interface IVolumeReader
    {
        public float Read(int2 offset);
    }

    public struct TextureSlope
    {
        public float X;
        public float Y;
        public float Height;
    }

    public interface ITextureSlopeReader
    {
        public TextureSlope Read(int2 offset);
    }

    static const int2 LEFT = int2(-1, 0);
    static const int2 RIGHT = int2(1, 0);
    static const int2 UP = int2(0, 1);
    static const int2 DOWN = int2(0, -1);

    #define FLOW_DIR_COUNT 4
    static const int2[FLOW_DIR_COUNT] FLOW_DIRS = int2[FLOW_DIR_COUNT](LEFT, RIGHT, UP, DOWN);

    static const float GRAVITY = 9.81f;
    static const float MAX_FLUX_ACCEL = 5.0f;
    static const float FLUX_DAMPING = 0.98f;

    public struct FluxUpdater<TVolumeReader : IVolumeReader, 
        TTextureSlopeReader : ITextureSlopeReader>
    {
        private TVolumeReader volumeReader;
        private TTextureSlopeReader slopeReader;
        private Physics.Flux2D currentFlux;
        private float textureSlopeIntensity;

        public __init(TVolumeReader _volumeReader, 
            TTextureSlopeReader _slopeReader,
            Physics.Flux2D _currentFlux,
            float _textureSlopeIntensity)
        {
            volumeReader = _volumeReader;
            slopeReader = _slopeReader;
            currentFlux =_currentFlux;
            textureSlopeIntensity = _textureSlopeIntensity;
        }

        [mutating]
        public Physics.Flux2D UpdateFlux()
        {
            float currentVolume = volumeReader.Read(0);
            
            [unroll(FLOW_DIR_COUNT)]
            for (int i = 0; i < FLOW_DIR_COUNT; i++)
            {
                int2 direction = FLOW_DIRS[i];
                float compareVolume = volumeReader.Read(direction);
                TextureSlope compareSlope = slopeReader.Read(direction);

                float acceleration = CalculateAcceleration(currentVolume, 
                    compareVolume,
                    compareSlope,
                    (float2)direction);

                currentFlux.SetByIndex(i, ApplyAcceleration(currentFlux.GetByIndex(i), acceleration));
            }

            currentFlux.Normalize(currentVolume);
            return currentFlux;
        }

        private float CalculateAcceleration(float currentVolume, 
            float compareVolume, 
            TextureSlope compareSlope,
            float2 direction)
        {
            float pressureDelta = currentVolume - compareVolume;
            float slopeInfluence = dot(direction, float2(compareSlope.X, compareSlope.Y));
            float totalForce = pressureDelta + (slopeInfluence * textureSlopeIntensity);
            float acceleration = totalForce * _DeltaTime * GRAVITY;
            
            return clamp(acceleration, -MAX_FLUX_ACCEL, MAX_FLUX_ACCEL);
        }

        private float ApplyAcceleration(float oldVal, float acceleration)
        {
            float newVal = oldVal + acceleration;
            newVal *= FLUX_DAMPING;
            return max(newVal, 0.0f);
        }
    }

    public interface IFluxResolver<TOutput, TFlow>
    {
        [mutating]
        public TOutput Resolve();
        public TFlow GetInFlow();
        public TFlow GetOutFlow();
    }

    public interface IFlux2DReader
    {
        public Physics.Flux2D Read(int2 offset);
    }

    public struct Flow
    {
        public float FluidAmount;
    }

    public struct PigmentFlow
    {
        public float4 PigmentAmount;
    }

    public struct FluxWaterResolver<TFluxReader : IFlux2DReader> : IFluxResolver<float, Flow>
    {
        TFluxReader fluxReader;
        Flow totalFlow;

        public __init(TFluxReader _fluxReader, float _volume)
        {
            fluxReader = _fluxReader;
            totalFlow.FluidAmount = _volume;
        }

        [mutating]
        public float Resolve()
        {
            Flow inFlow = GetInFlow();
            Flow outFlow = GetOutFlow();
            totalFlow.FluidAmount += inFlow.FluidAmount - outFlow.FluidAmount;

            return max(totalFlow.FluidAmount, 0.0f);
        }

        private Flow GetInFlow()
        {
            Flow inFlow = (Flow)0;

            [unroll(FLOW_DIR_COUNT)]
            for (int i = 0; i < FLOW_DIR_COUNT; i++)
            {
                Physics.Flux2D flux = fluxReader.Read(FLOW_DIRS[i]);
                inFlow.FluidAmount += flux.GetByIndex(Physics.Flux2D.GetOpposite(i));
            }

            return inFlow;
        }

        private Flow GetOutFlow()
        {
            Physics.Flux2D flux = fluxReader.Read(0);
            Flow outFlow = (Flow)0;
            outFlow.FluidAmount = flux.GetSum();
            return outFlow;
        }
    }

    public interface IPigmentReader
    {
        public float4 Read(int2 offset);
    }

    public struct PigmentResolver<TVolumeReader : IVolumeReader,
        TFluxReader : IFlux2DReader,
        TPigmentReader : IPigmentReader> : IFluxResolver<float4, PigmentFlow>
    {
        private TVolumeReader volumeReader;
        private TFluxReader fluxReader;
        private TPigmentReader pigmentReader;
        private PigmentFlow totalFlow;

        public __init(TVolumeReader _volumeReader,
            TFluxReader _fluxReader,
            TPigmentReader _pigmentReader,
            float4 _pigmentAmount)
        {
            volumeReader = _volumeReader;
            fluxReader = _fluxReader;
            pigmentReader = _pigmentReader;
            totalFlow.PigmentAmount = _pigmentAmount;
        }

        [mutating]
        public float4 Resolve()
        {
            PigmentFlow inFlow = GetInFlow();
            PigmentFlow outFlow = GetOutFlow();
            
            totalFlow.PigmentAmount += inFlow.PigmentAmount - outFlow.PigmentAmount;
        
            return max(totalFlow.PigmentAmount, 0.0f);
        }

        private PigmentFlow GetInFlow()
        {
            PigmentFlow inFlow = (PigmentFlow)0;
            inFlow.PigmentAmount = 0;

            [unroll(FLOW_DIR_COUNT)]
            for (int d = 0; d < FLOW_DIR_COUNT; d++)
            {
                int2 dir = FLOW_DIRS[d];
                
                float inVolume = volumeReader.Read(dir);
                float4 inPigment = pigmentReader.Read(dir);
                float4 inConcentration = GetConcentration(inPigment, inVolume);

                Physics.Flux2D inFlux = fluxReader.Read(dir);
                float waterGained = inFlux.GetByIndex(Physics.Flux2D.GetOpposite(d));

                inFlow.PigmentAmount += inConcentration * waterGained;
            }

            return inFlow;
        }

        private PigmentFlow GetOutFlow()
        {
            Physics.Flux2D myFlux = fluxReader.Read(0);
            float currentFluxFlow = myFlux.GetSum();

            float currentVolume = volumeReader.Read(0);
            float4 currentConcentration = GetConcentration(totalFlow.PigmentAmount, currentVolume);

            PigmentFlow outFlow = (PigmentFlow)0;
            outFlow.PigmentAmount = currentFluxFlow * currentConcentration;
            
            return outFlow;
        }

        private float4 GetConcentration(float4 pigment, float volume)
        {
            float safeVolume = max(volume, 1e-9f);
            float4 rawConcentration = saturate(pigment / safeVolume);

            float minFlowVolume = 1e-8f; 
            float maxFlowVolume = 1e-3f; 

            float carryCapacity = smoothstep(minFlowVolume, maxFlowVolume, volume);
            carryCapacity = pow(carryCapacity, 0.1f);
            
            return rawConcentration * carryCapacity;
        }
    }
}