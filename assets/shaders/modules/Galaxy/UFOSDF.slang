#include "Core/BeerMacros.slang"

import SDF;

module UFOSDF;

namespace SDF
{
    static const float SAUCER_SMOOTHNESS = 0.03f;
    static const float SAUCER_SQUISH = 0.6f;
    static const float RIM_SMOOTHNESS = 0.03f;
    static const float RIM_SQUISH = 1.2f;
    static const float RIM_ADD = 0.535f;
    static const float BIG_GROOVE_FREQUENCY = 5.0f;
    static const float BIG_GROOVE_STRENGTH = 0.023f;
    static const float SMALL_GROOVE_FREQUENCY = 30.0f;
    static const float SMALL_GROOVE_STRENGTH = 0.003f;

    static const float FINS_LENGTH = 0.4f;
    static const float FINS_THICKNESS = 0.02f;
    static const float FINS_HEIGHT = 0.06f;
    static const float FINS_BEVEL = 0.01f;
    static const float3 FINS_POS = float3(0.4f, 0.23f, -0.25f);

    static const float ALIEN_BODY_HEIGHT = 0.25f;
    static const float ALIEN_BODY_SMOOTHNESS = 0.03f;
    static const float ALIEN_BODY_STRETCH = rcp(3.0f);
    static const float ALIEN_BODY_RADIUS = 0.07f;
    static const float ALIEN_HEAD_HEIGHT = 0.35f;
    static const float ALIEN_HEAD_RADIUS = 0.12f;
    static const float ALIEN_ANTENNA_OFFSET = 0.135f;
    static const float ALIEN_ANTENNA_TAIL_RADIUS = 0.03f;

    static const float COCKPIT_RADIUS = 0.35f;
    static const float COCKPIT_HEIGHT = 0.3f;

    static const float SAUCER_MAT = 0.0f;
    static const float FINS_MAT = 1.0f;
    static const float ALIEN_MAT = 2.0f;

public struct Saucer : SDF
{
    private float3 center;
    private float scale;
    private float invScale;
    private float3x3 invRotation;
    private float cutoff;
    private float3 saucerColor;
    private float3 finsColor;
    private float3 alienColor;

    public __init(float3 _center, 
        float _scale, 
        float4x4 rotation,
        float _cutoff,
        float3 _saucerColor,
        float3 _finsColor,
        float3 _alienColor)
    {
        center = _center;
        scale = _scale;
        invScale = rcp(scale);
        invRotation = (float3x3)transpose(rotation);
        cutoff = (_cutoff - 0.5f) * 2.0f;
        saucerColor = _saucerColor;
        finsColor = _finsColor;
        alienColor = _alienColor;
    }

    public FieldOut Evaluate(MarchData data)
    {
        float3 localPos = (data.Position - center) * invScale;
        FieldOut saucerDist = Saucer(localPos);
        saucerDist.SignedDist *= scale;

        return saucerDist;
    }

    private FieldOut Saucer(float3 localPos)
    {
        float3 rotatedPos = mul(invRotation, localPos);
        float3 hullCoord = rotatedPos;

        hullCoord.y = sqrt(hullCoord.y * hullCoord.y + SAUCER_SMOOTHNESS);
        hullCoord.y += SAUCER_SQUISH;

        float saucer = Functions.Sphere(hullCoord, 1.0f); 

        float radialGrad = atan2(hullCoord.x, hullCoord.z);

        saucer += GetGrooveModifier(radialGrad,
            BIG_GROOVE_STRENGTH,
            BIG_GROOVE_FREQUENCY,
            0.8f,
            0.9f);

        saucer += GetGrooveModifier(radialGrad,
            SMALL_GROOVE_STRENGTH,
            SMALL_GROOVE_FREQUENCY,
            0.9f,
            1.0f);

        float rim = GetRim(rotatedPos);

        float base = Functions.SmoothMin(FieldOut(saucer), FieldOut(rim), 0.01f).SignedDist; 
        float cockpit = Cockpit.GetCockpit(rotatedPos);
        float fins = GetFins(rotatedPos);
        float alien = GetAlien(rotatedPos);

        FieldOut fullSaucer = Functions.Union(Functions.Subtraction(FieldOut(cockpit, SAUCER_MAT), FieldOut(base, SAUCER_MAT)), FieldOut(fins, FINS_MAT));
        fullSaucer = Functions.Union(fullSaucer, FieldOut(alien, ALIEN_MAT));

        ApplyCutoff(fullSaucer.SignedDist, rotatedPos, cutoff);

        return fullSaucer;
    }

    private float GetGrooveModifier(float radialGrad,
        float strength, 
        float frequency,
        float edgeMin,
        float edgeMax)
    {
        float sinGrad = sin(radialGrad * frequency);
        return smoothstep(edgeMin, edgeMax, sinGrad) * strength;
    }

    private float GetRim(float3 rotatedPos)
    {
        float3 hullCoord = rotatedPos;
        hullCoord.y = sqrt(hullCoord.y * hullCoord.y + RIM_SMOOTHNESS);
        hullCoord.y += RIM_SQUISH;

        return Functions.Sphere(hullCoord, 1.0f + RIM_ADD);
    }

    private float GetFins(float3 rotatedPos)
    {
        float3 mirrorPos = float3(abs(rotatedPos.x), rotatedPos.yz);
        float3 boxPos = mirrorPos - FINS_POS;
        float box = Functions.Box(boxPos, float3(FINS_THICKNESS, FINS_HEIGHT, FINS_LENGTH));

        float3 cutNormal = normalize(float3(0.0f, -3.0f, -1.0f)); 
        float3 planePos = boxPos - float3(0.0f, 0.0f, -FINS_LENGTH * 0.8f); 
        float cutPlane = dot(planePos, cutNormal);

        float cutBox = max(box, cutPlane);

        return cutBox - FINS_BEVEL;
    }

    private float GetAlien(float3 rotatedPos)
    {
        float3 hullCoord = rotatedPos;
        hullCoord.y -= ALIEN_BODY_HEIGHT;
        hullCoord.y = sqrt(hullCoord.y * hullCoord.y + ALIEN_BODY_SMOOTHNESS);
        hullCoord.y *= ALIEN_BODY_STRETCH;
        
        float body = Functions.Sphere(hullCoord, ALIEN_BODY_RADIUS);
        
        float3 headCoord = rotatedPos;
        headCoord.y -= ALIEN_HEAD_HEIGHT;
        float head = Functions.Sphere(headCoord, ALIEN_HEAD_RADIUS);

        float3 antennaCoord = headCoord;
        antennaCoord.x = abs(antennaCoord.x);

        float3 antennaOffset = float3(ALIEN_ANTENNA_OFFSET, ALIEN_ANTENNA_OFFSET, 0);
        float antennaBase = Functions.Line(antennaCoord, 0, antennaOffset);
        float antennaTail = Functions.Sphere(antennaCoord - ALIEN_ANTENNA_OFFSET, ALIEN_ANTENNA_TAIL_RADIUS);
        float antenna = Functions.SmoothMin(FieldOut(antennaBase), FieldOut(antennaTail), 0.03f).SignedDist;

        float headBody = Functions.SmoothMin(FieldOut(body), FieldOut(head), 0.01f).SignedDist;
        float alien = Functions.Union(FieldOut(headBody), FieldOut(antenna)).SignedDist;

        return alien;
    }

    public static void ApplyCutoff(inout float dist, float3 rotatedPos, float time)
    {
        float cutter = rotatedPos.y - time;
        dist = Functions.Intersection(FieldOut(dist), FieldOut(cutter)).SignedDist;
    } 

    public float3 GetColor(float material)
    {
        float3 color = lerp(saucerColor, finsColor, clamp(material, 0.0f, 1.0f));
        color = lerp(color, alienColor, clamp(material - 1.0f, 0.0f, 1.0f));
        
        return color;
    }
}

    public struct Cockpit : SDF
    {
        private float3 center;
        private float scale;
        private float invScale;
        private float3x3 invRotation;
        private float cutoff;

        public __init(float3 _center, 
            float _scale, 
            float4x4 rotation,
            float _cutoff)
        {
            center = _center;
            scale = _scale;
            invScale = rcp(scale);
            invRotation = (float3x3)transpose(rotation);
            cutoff = (_cutoff - 0.5f) * 2.0f;
        }
    
        public FieldOut Evaluate(MarchData data)
        {
            float3 localPos = (data.Position - center) * invScale;
            FieldOut cockPitDist = Cockpit(localPos);
            cockPitDist.SignedDist *= scale;
            return cockPitDist;
        }

        private FieldOut Cockpit(float3 localPos)
        {
            float3 rotatedPos = mul(invRotation, localPos);
            float cockpit = GetCockpit(rotatedPos);
            Saucer::ApplyCutoff(cockpit, rotatedPos, cutoff);
            return FieldOut(cockpit);
        }

        public static float GetCockpit(float3 rotatedPos)
        {
            float3 rotatedOffset = float3(0, COCKPIT_HEIGHT, 0);
            return Functions.Sphere(rotatedPos - rotatedOffset, COCKPIT_RADIUS);
        }
    }

    public float GetGlowGradient(float3 positionOS, 
        float cutoff,
        float thickness)
    {
        cutoff = (cutoff - 0.5f) * 2.0f;
        return 1.0f - smoothstep(cutoff, cutoff - thickness, positionOS.y);
    }
}