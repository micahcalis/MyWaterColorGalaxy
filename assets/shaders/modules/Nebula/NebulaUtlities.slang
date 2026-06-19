#include "Core/BeerMacros.slang"

import Core.BeerCore;

public struct ObjectData
{
    public float4 ColorA;
    public float4 ColorB;
    public float4 ColorC;
    public float4 ColorD;
    public float Scale;
    public uint Id;
}

namespace Nebula
{
    #define MAX_NEBULA_TILES 31

    public struct NebulaTile
    {
        public uint NebulaCount;   
        public uint NebulaIndices[MAX_NEBULA_TILES];
    }

    public uint FlattenTileId(uint2 id, uint2 tileResolution)
    {
        return id.x + id.y * tileResolution.x;
    }

    public bool ThreadOutOfBounds(uint2 id, uint2 tileResolution)
    {
        return id.x >= tileResolution.x
            || id.y >= tileResolution.y;
    }

    public struct ObjectPair
    {
        public float3 Position;
        public float Scale;
        public int Id;
    }

    enum FrustumPlane : uint
    {
        Left = 0,
        Right = 1,
        Top = 2,
        Bottom = 3
    }

    public struct FrustumPlanes
    {
        public float3[4] Planes;
    }

    public interface INebulaObjectReader
    {
        public ObjectPair GetObject(int id);
    }

    public struct NebulaTileComputer<TObjectReader : INebulaObjectReader>
    {
        private uint2 tileId;
        private int stardustCount;
        private float2 numTiles;
        TObjectReader objectReader;

        public __init(uint2 _tileId,
            int _stardustCount,
            float2 _numTiles,
            TObjectReader _objectReader)
        {
            tileId = _tileId;
            stardustCount = _stardustCount;
            numTiles = _numTiles;
            objectReader = _objectReader;
        }

        public NebulaTile Compute()
        {
            FrustumPlanes planes = GetFrustumPlanes();
            NebulaTile tile = (NebulaTile)0;

            for (int i = 0; i < stardustCount; i++)
            {
                ObjectPair currentPair = objectReader.GetObject(i);

                if (IsInside(currentPair, planes))
                {
                    tile.NebulaIndices[tile.NebulaCount] = i;
                    tile.NebulaCount++;

                    if (tile.NebulaCount >= MAX_NEBULA_TILES)
                    {
                        break;
                    }
                }
            }

            return tile;
        }   

        private FrustumPlanes GetFrustumPlanes()
        {
            float2 uvTopLeft = float2(tileId) / numTiles;
            float2 uvBottomRight = float2(tileId + 1) / numTiles;

            float ndcLeft   = uvTopLeft.x * 2.0 - 1.0;
            float ndcRight  = uvBottomRight.x * 2.0 - 1.0;

            float ndcTop    = uvTopLeft.y * 2.0 - 1.0;
            float ndcBottom = uvBottomRight.y * 2.0 - 1.0;

            float3 topLeft_VS     = ClipToView(float2(ndcLeft, ndcTop));
            float3 topRight_VS    = ClipToView(float2(ndcRight, ndcTop));
            float3 bottomLeft_VS  = ClipToView(float2(ndcLeft, ndcBottom));
            float3 bottomRight_VS = ClipToView(float2(ndcRight, ndcBottom));

            FrustumPlanes planes;
            planes.Planes[FrustumPlane.Left]   = normalize(cross(bottomLeft_VS,  topLeft_VS));
            planes.Planes[FrustumPlane.Right]  = normalize(cross(topRight_VS,    bottomRight_VS));
            planes.Planes[FrustumPlane.Top]    = normalize(cross(topLeft_VS,     topRight_VS));
            planes.Planes[FrustumPlane.Bottom] = normalize(cross(bottomRight_VS, bottomLeft_VS));

            return planes;
        }

        private bool IsInside(ObjectPair pair, FrustumPlanes planes)
        {
            bool isInside = true;
            float3 centerVS = mul(_ViewMat, float4(pair.Position, 1.0f)).xyz;

            for (int i = 0; i < 4; i++)
            {
                if (dot(planes.Planes[i], centerVS) < -pair.Scale)
                {
                    isInside = false;
                    break;
                }
            }

            return isInside;
        }

        private static float3 ClipToView(float2 ndc)
        {
            float4 clipSpace = float4(ndc, 1.0, 1.0);
            float4 viewSpace = mul(_InvProjMat, clipSpace);
        
            return viewSpace.xyz / viewSpace.w;
        }
    }

    public float GetDistortion(float wetness)
    {
        return pow(wetness, 0.75f) * 0.5f + 0.5f;
    }
}