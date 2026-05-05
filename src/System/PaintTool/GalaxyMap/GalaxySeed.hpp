#pragma once

#include "glm/glm.hpp"
#include <cstdint>
#include <print>

namespace Beer::System
{
    struct GalaxySeed
    {
    public:
        uint32_t StarSeed = 0;
        uint32_t ColorSeed = 0;

    public:
        GalaxySeed()
        {
            StarSeed = static_cast<uint32_t>(rand());
            ColorSeed = static_cast<uint32_t>(rand());
        }
    };

    static const float MIN_STAR_SIZE = 0.3f;
    static const float MAX_STAR_SIZE = 0.5f;
    static const glm::vec2 STAR_BOUNDS_MIN = glm::vec2(0.4f, 0.4f);
    static const glm::vec2 STAR_BOUNDS_MAX = glm::vec2(0.6f, 0.6f);

    static constexpr uint32_t HashInt(uint32_t x)
    {
        x ^= x >> 16;
        x *= 0x85ebca6bu;
        x ^= x >> 13;
        x *= 0xc2b2ae35u;
        x ^= x >> 16;
        return x;
    }

    static constexpr glm::vec3 RGBtoHSV(glm::vec3 rgb)
    {
        glm::vec3 c = rgb;
        glm::vec4 k = glm::vec4(0.0f, -1.0f / 3.0f, 2.0f / 3.0f, -1.0f);
        glm::vec4 p = glm::mix(glm::vec4(c.b, c.g, k.w, k.z), glm::vec4(c.g, c.b, k.x, k.y), glm::step(c.b, c.g));
        glm::vec4 q = glm::mix(glm::vec4(p.x, p.y, p.w, c.r), glm::vec4(c.r, p.y, p.z, p.x), glm::step(p.x, c.r));

        float d = q.x - glm::min(q.w, q.y);
        float e = 1.0e-10f;

        return glm::vec3(glm::abs(q.z + (q.w - q.y) / (6.0f * d + e)), d / (q.x + e), q.x);
    }

    static constexpr glm::vec3 HSVtoRGB(glm::vec3 hsv)
    {
        glm::vec3 c = hsv;
        glm::vec4 k = glm::vec4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
        glm::vec3 p = glm::abs(glm::fract(glm::vec3(c.x) + glm::vec3(k.x, k.y, k.z)) * 6.0f - glm::vec3(k.w));
        return c.z * glm::mix(glm::vec3(k.x), glm::clamp(p - glm::vec3(k.x), 0.0f, 1.0f), c.y);
    }

    static const float MIN_DUST_SATURATION = 0.75f;
    static const float MAX_DUST_SATURATION = 1.0f;
    static const float MIN_DUST_VALUE = 0.8f;
    static const float MAX_DUST_VALUE = 0.95f;

    static const float MIN_STAR_SATURATION = 0.2f;
    static const float MAX_STAR_SATURATION = 0.9f;
    static const float MIN_STAR_VALUE = 0.85f;
    static const float MAX_STAR_VALUE = 0.95f;

    static constexpr float Remap(float x, glm::vec2 a, glm::vec2 b)
    {
        return b.x + (x - a.x) * (b.y - b.x) / (a.y - a.x);
    }

    static constexpr glm::vec4 TransformGalaxyColor(glm::vec4 base, bool isStar)
    {
        glm::vec2 saturationRange = isStar
            ? glm::vec2(MIN_STAR_SATURATION, MAX_STAR_SATURATION)
            : glm::vec2(MIN_DUST_SATURATION, MAX_DUST_SATURATION);

        glm::vec2 valueRange = isStar
            ? glm::vec2(MIN_STAR_VALUE, MAX_STAR_VALUE)
            : glm::vec2(MIN_DUST_VALUE, MAX_DUST_VALUE);

        base.g = Remap(base.g, glm::vec2(0, 1), saturationRange);
        base.b = Remap(base.b, glm::vec2(0, 1), valueRange);
        glm::vec3 rgb = HSVtoRGB(glm::vec3(base.r, base.g, base.b));
        return glm::vec4(rgb.r, rgb.g, rgb.b, base.a);
    }

    struct GalaxySettings
    {
    public:
        glm::vec4 StarColor{};
        glm::vec2 StarPosition{};
        float StarSize{};

        glm::vec4 ColorA{};
        glm::vec4 ColorB{};
        glm::vec4 ColorC{};

    public:
        void UpdateSettings(const GalaxySeed seed)
        {
            auto nextFloat = [](uint32_t& state) -> float {
                state = HashInt(state);
                return static_cast<float>(state) / static_cast<float>(std::numeric_limits<uint32_t>::max());
            };

            auto nextColor = [&nextFloat](uint32_t& state, bool isStar) -> glm::vec4 {
                return TransformGalaxyColor(
                    glm::vec4(nextFloat(state),
                        nextFloat(state),
                        nextFloat(state),
                        1.0f),
                    isStar);
            };

            uint32_t starState = seed.StarSeed;

            StarPosition = glm::mix(
                STAR_BOUNDS_MIN,
                STAR_BOUNDS_MAX,
                glm::vec2(nextFloat(starState), nextFloat(starState)));

            StarSize = glm::mix(MIN_STAR_SIZE,
                MAX_STAR_SIZE,
                nextFloat(starState));

            uint32_t colorState = seed.ColorSeed;

            StarColor = nextColor(colorState, true);
            ColorA = nextColor(colorState, false);
            ColorB = nextColor(colorState, false);
            ColorC = nextColor(colorState, false);
        }
    };
} // namespace Beer::System
