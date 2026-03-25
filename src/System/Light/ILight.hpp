#pragma once

#include "glm/glm.hpp"
#include <memory>

namespace Beer::System
{
    class LightManager;
}

namespace Beer::System
{
    class ILight
    {
    public:
        virtual ~ILight() = default;
        virtual glm::vec3 GetPosition() const = 0;
        virtual glm::vec4 GetDirectColor() const = 0;
        virtual glm::vec4 GetShadowColor() const = 0;
        virtual glm::vec4 GetAmbientColor() const = 0;
        virtual int GetPriority() const = 0;

    private:
        inline static LightManager* lightManager = nullptr;

    public:
        static void SetLightManager(LightManager* manager) { lightManager = manager; }

        template<typename T, typename... Args>
        static std::unique_ptr<T> CreateLight(Args&&... args);
        static ILight* Main();
    };
} // namespace Beer::System

#include "ILight.inl"
