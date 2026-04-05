    #include "BeerMacros.slang"

    import BeerCore;

    module BlitUtils;

    namespace Blit
    {
        public struct BlitTriangle
        {
            private uint vertexId;

            public __init(uint _vertexId)
            {
                vertexId = _vertexId;
            }

            public void FillData(out float4 positionCS, out float2 uv)
            {   
                uv = float2((vertexId << 1) & 2, vertexId & 2);
                positionCS = float4(uv * 2.0f - 1.0f, 0.5f, 1.0f);
            }
        }
    }