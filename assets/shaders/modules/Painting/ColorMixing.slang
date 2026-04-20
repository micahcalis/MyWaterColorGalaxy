#include "Core/BeerMacros.slang"

import Core.BeerCore;

module ColorMixing;

namespace Painting
{
    public struct Canvas
    {
        private float2 topRight;
        private float2 topLeft;
        private float2 botRight;
        private float2 botLeft;

        public __init(float2 _topRight,
            float2 _topLeft,
            float2 _botRight,
            float2 _botLeft)
        {
            topRight = _topRight;
            topLeft = _topLeft;
            botRight = _botRight;
            botLeft = _botLeft;
        }

        public bool InCanvas(float2 position)
        {
            bool2 posCheck = position >= botLeft && position < topRight;
            return posCheck.x && posCheck.y;
        }
    }

}