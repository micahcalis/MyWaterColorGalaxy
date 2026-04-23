#include "Core/BeerMacros.slang"

import Core.BeerCore;

module Flux;

namespace Physics
{
    public interface IFlux
    {
        public float GetSum();

        [mutating]
        public void Scale(float scale);
    }

    public struct Flux2D : IFlux
    {
        public float Up;
        public float Down;
        public float Right;
        public float Left;

        public float GetSum()
        {
            return Up + Down + Right + Left;
        }

        [mutating]
        public void Scale(float scale)
        {
            Up *= scale;
            Down *= scale;
            Right *= scale;
            Left *= scale;
        }

        [mutating]
        public void Normalize(float volume)
        {
            float fluxSum = GetSum();
            float normVal = fluxSum < volume ? 1 :
                volume / max(fluxSum, 1e-4);

            Scale(normVal);
        }

        public float GetByIndex(int index)
        {
            switch (index)
            {
                case 0: return Left;
                case 1: return Right;
                case 2: return Up;
                case 3: return Down;
                default: return Left;
            }
        }

        [mutating]
        public void SetByIndex(int index, float value)
        {
            switch (index)
            {
                case 0: Left = value; break;
                case 1: Right = value; break;
                case 2: Up = value; break;
                case 3: Down = value; break;
            }
        }

        public static int GetOpposite(int index)
        {
            return index ^ 1;
        }
    }
} 