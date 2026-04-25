#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Painting.Pigment;

module KubelkaMunk;

namespace Painting
{
    #define NUM_PIGMENTS 12

    namespace Functions
    {
        float3 GetInteractionRatio(PigmentProperties properties)
        {
            return 1 + properties.K / properties.S;
        }

        float3 GetAttenuationRoot(float3 interactionRatio)
        {
            return sqrt(max((interactionRatio * interactionRatio) - 1.0f, 0.0f));
        }
    }

    public struct OpticalLayer
    {
        public float3 Reflectance;
        public float3 Transmittance;

        public __init(float3 _reflectance, float3 _transmittance)
        {
            Reflectance = _reflectance;
            Transmittance = _transmittance;
        }
    }

    public struct MunkSolver
    {
        private Painting.PigmentProperties properties;
        private float thickness;

        public __init(Painting.PigmentProperties _properties,
            float _thickness)
        {
            properties = _properties;
            properties.S = max(properties.S, 1e-4f);
            thickness = _thickness;
        }

        public OpticalLayer Solve()
        {
            float3 a = Functions.GetInteractionRatio(properties);
            float3 b = Functions.GetAttenuationRoot(a);

            float3 pigmentTerm = b * properties.S * thickness;
            pigmentTerm = min(pigmentTerm, 50.0f);
            float3 sinhP = sinh(pigmentTerm);
            float3 coshP = cosh(pigmentTerm);

            float3 denominator = a * sinhP + b * coshP;
            denominator = max(denominator, 1e-4f);

            return OpticalLayer(sinhP / denominator, b / denominator);
        }
    }

    public interface IThicknessReader
    {
        public float Read(int index);
    }

    public struct KubelkaMunkRenderer<TThicknessReader: IThicknessReader>
    {
        private TThicknessReader thicknessReader;
        private float3 paperColor;

        public __init(TThicknessReader _thicknessReader,
            float3 _paperColor)
        {
            thicknessReader = _thicknessReader;
            paperColor = _paperColor;
        }

        public float3 Render()
        {
            OpticalLayer paintLayer = CalculatePaintLayer();
            OpticalLayer paperLayer = OpticalLayer(paperColor, 0);

            float3 numerator = paintLayer.Transmittance * paintLayer.Transmittance * paperLayer.Reflectance;
            float3 denominator = 1.0f - paintLayer.Reflectance * paperLayer.Reflectance;
            denominator = max(denominator, 1e-4f);

            return paintLayer.Reflectance + numerator / denominator;
        }

        private OpticalLayer CalculatePaintLayer()
        {
            Painting.PigmentProperties properties; float thickness;
            CombineProperties(properties, thickness);

            MunkSolver solver = MunkSolver(properties, thickness);
            return solver.Solve();
        }

        private void CombineProperties(out Painting.PigmentProperties properties, out float thickness)
        {
            properties = Painting.PigmentProperties(0, 0, 0, 0 ,0);
            thickness = 1e-4f;

            [unroll(NUM_PIGMENTS)]
            for (int i = 0; i < NUM_PIGMENTS; i++)
            {
                float currentThickness = thicknessReader.Read(i);

                if (currentThickness < 1e-4f)
                    continue;

                Painting.PigmentProperties combine = Painting.GetPigmentProperties((Painting.Pigment)i);
                properties.K += combine.K * currentThickness;
                properties.S += combine.S * currentThickness;
                thickness += currentThickness;
            }

            properties.K /= thickness;
            properties.S /= thickness;
        }
    }
}