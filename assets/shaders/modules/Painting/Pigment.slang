#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Painting.FluidPhysics;

module Pigment;

namespace Painting
{
    #define NUM_PIGMENTS 12

    public enum Pigment : int
    {
        QuinacridoneRose = 0, 
        IndianRed = 1,
        CadmiumYellow = 2, 
        HookersGreen = 3,
        CeruleanBlue = 4,
        BurntUmber = 5,
        CadmiumRed = 6,
        BrilliantOrange = 7,
        HansaYellow = 8,
        PhthaloGreen = 9,
        FrenchUltramarine = 10,
        InterferenceLilac= 11
    };

    public struct PigmentProperties
    {
        public float3 K;
        public float3 S;
        public float Density;
        public float StainingPower;
        public float Granulation;

        public __init(float3 _K, float3 _S, float _Density, float _StainingPower, float _Granulation)
        {
            K = _K;
            S = _S;
            Density = _Density;
            StainingPower = _StainingPower;
            Granulation = _Granulation;
        }
    }

    const static PigmentProperties[NUM_PIGMENTS] PIGMENT_PROP_LIST = PigmentProperties[NUM_PIGMENTS](
        // 0: Quinacridone Rose
        PigmentProperties(float3(0.22f, 1.47f, 0.57f), float3(0.05f, 0.003f, 0.03f), 0.02f, 5.5f, 0.81f),
        // 1: Indian Red
        PigmentProperties(float3(0.46f, 1.07f, 1.50f), float3(1.28f, 0.38f, 0.21f), 0.05f, 7.0f, 0.40f),
        // 2: Cadmium Yellow
        PigmentProperties(float3(0.10f, 0.36f, 3.45f), float3(0.97f, 0.65f, 0.007f), 0.05f, 3.4f, 0.81f),
        // 3: Hookers Green
        PigmentProperties(float3(1.62f, 0.61f, 1.64f), float3(0.01f, 0.012f, 0.003f), 0.09f, 1.0f, 0.41f),
        // 4: Cerulean Blue
        PigmentProperties(float3(1.52f, 0.32f, 0.25f), float3(0.06f, 0.26f, 0.40f), 0.01f, 1.0f, 0.31f),
        // 5: Burnt Umber
        PigmentProperties(float3(0.74f, 1.54f, 2.10f), float3(0.09f, 0.09f, 0.004f), 0.09f, 9.3f, 0.90f),
        // 6: Cadmium Red
        PigmentProperties(float3(0.14f, 1.08f, 1.68f), float3(0.77f, 0.015f, 0.018f), 0.02f, 1.0f, 0.63f),
        // 7: Brilliant Orange
        PigmentProperties(float3(0.13f, 0.81f, 3.45f), float3(0.005f, 0.009f, 0.007f), 0.01f, 1.0f, 0.14f),
        // 8: Hansa Yellow
        PigmentProperties(float3(0.06f, 0.21f, 1.78f), float3(0.50f, 0.88f, 0.009f), 0.06f, 1.0f, 0.08f),
        // 9: Phthalo Green
        PigmentProperties(float3(1.55f, 0.47f, 0.63f), float3(0.01f, 0.05f, 0.035f), 0.02f, 1.0f, 0.12f),
        // 10: French Ultramarine
        PigmentProperties(float3(0.86f, 0.86f, 0.06f), float3(0.005f, 0.005f, 0.09f), 0.01f, 3.1f, 0.91f),
        // 11: Interference Lilac
        PigmentProperties(float3(0.08f, 0.11f, 0.07f), float3(1.25f, 0.42f, 1.43f), 0.06f, 1.0f, 0.08f));

    public PigmentProperties GetPigmentProperties(Pigment pigment)
    {
        int index = clamp((int)pigment, 0, NUM_PIGMENTS - 1);
        return PIGMENT_PROP_LIST[index];
    }

    public struct PigmentLocation
    {
        public int Layer;
        public int ChannelIndex;

        public __init(Pigment pigment)
        {
            Layer = (int)pigment / 4;
            ChannelIndex = (int)pigment % 4;
        }
    }

    public Pigment GetPigmentFromThread(int idz, int index)
    {
        return (Pigment)(idz * 4 + index);
    }

    public interface IPigmentReader
    {
        public float Read();
    }

    public struct TransferOutput
    {
        public float Suspended;
        public float Deposited;
    }

    public struct PigmentTransferer<TSuspendedReader : IPigmentReader,
        TDepositedReader : IPigmentReader,
        TTextureSlopeReader : ITextureSlopeReader>
    {
        private TSuspendedReader suspendedReader;
        private TDepositedReader depositedReader;
        private TTextureSlopeReader textureSlopeReader;
        private PigmentProperties properties;
        private float transferSpeed;

        public __init(TSuspendedReader _suspendedReader, 
            TDepositedReader _depositedReader,
            TTextureSlopeReader _textureSlopeReader,
            Pigment _pigment, 
            float _transferSpeed)
        {
            suspendedReader = _suspendedReader;
            depositedReader = _depositedReader;
            textureSlopeReader = _textureSlopeReader;
            properties = GetPigmentProperties(_pigment);
            transferSpeed = _transferSpeed;
        }
        
        public TransferOutput Transfer()
        {
            float suspended = suspendedReader.Read();
            float deposited = depositedReader.Read();
            float canvasHeight = textureSlopeReader.Read(0).Height;

            float delta = transferSpeed * _DeltaTime;

            float pigmentDown = suspended * (1.0f - (canvasHeight * properties.Granulation)) 
                * properties.Density * delta;

            float pigmentUp = deposited * (1.0f + (canvasHeight - 1.0f) * properties.Granulation) 
                * properties.Density / properties.StainingPower * delta;

            pigmentDown = min(pigmentDown, suspended);
            pigmentUp = min(pigmentUp, deposited);

            if (deposited + pigmentDown > 1)
            {
                pigmentDown = max(0.0f, 1.0f - deposited);
            }

            if (suspended + pigmentUp > 1)
            {
                pigmentUp = max(0.0f, 1.0f - suspended);
            }

            TransferOutput output;
            output.Deposited = deposited + pigmentDown - pigmentUp;
            output.Suspended = suspended + pigmentUp - pigmentDown;
            
            return output;
        }
    }
}