#pragma once

#include "Vendor/nlohmann/json.hpp"
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace glm
{
    inline void to_json(nlohmann::json& j, const glm::vec2& v)
    {
        j = nlohmann::json{{"x", v.x}, {"y", v.y}};
    }
    inline void from_json(const nlohmann::json& j, vec2& v)
    {
        j.at("x").get_to(v.x);
        j.at("y").get_to(v.y);
    }
    inline void to_json(nlohmann::json& j, const vec4& v)
    {
        j = nlohmann::json{{"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w}};
    }
    inline void from_json(const nlohmann::json& j, vec4& v)
    {
        j.at("x").get_to(v.x);
        j.at("y").get_to(v.y);
        j.at("z").get_to(v.z);
        j.at("w").get_to(v.w);
    }
} // namespace glm

namespace Beer::System
{

    struct SerializableGalaxyComponent
    {
        uint32_t Id = 0;
        uint32_t TypeIndex = 0;
        std::vector<glm::vec4> Colors;
        float Scale = 0;
        glm::vec2 Position = glm::vec2(0);
    };

    class SerializableGalaxy
    {
    public:
        uint32_t StarSeed = 0;
        uint32_t ColorSeed = 0;

        glm::vec4 StarColor{};
        glm::vec2 StarPosition{};
        float StarSize = 0;

        glm::vec4 ColorA{};
        glm::vec4 ColorB{};
        glm::vec4 ColorC{};

        std::vector<SerializableGalaxyComponent> Components;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SerializableGalaxyComponent, Id, TypeIndex, Colors, Scale, Position)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SerializableGalaxy, StarSeed, ColorSeed, StarColor, StarPosition, StarSize, ColorA, ColorB, ColorC, Components)
} // namespace Beer::System
