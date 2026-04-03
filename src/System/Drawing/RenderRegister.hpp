#pragma once

#include "Core/Application/Renderer/DrawCallPool.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Drawing/DrawRequest.hpp"
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace Beer::System
{
    class RenderRegister
    {
    private:
        std::unordered_map<ContextType, std::vector<IRenderComponent*>> componentsMap;

    public:
        RenderRegister();
        void AddComponent(IRenderComponent* component, const ContextType context);
        void RemoveFromRegister(IRenderComponent* component);
        void Cleanup();
        Core::DrawCallPool GetDrawCallPool(const DrawRequest& request);

    private:
        bool IsRenderComponentValid(const DrawRequest& request, IRenderComponent* component);

    private:
        inline static RenderRegister* renderRegister = nullptr;

    public:
        static void SetRenderRegister(RenderRegister* renderRegister)
        {
            RenderRegister::renderRegister = renderRegister;
        }

        template<typename T, typename... Args>
        static std::unique_ptr<T> CreateRenderComponent(const ContextType context, Args&&... args)
        {
            static_assert(std::is_base_of_v<IRenderComponent, T>, "T must inherit from IRenderComponent");

            if (RenderRegister::renderRegister == nullptr)
                throw std::runtime_error("Render Register not Initialized");

            std::unique_ptr<T> component = std::make_unique<T>(std::forward<Args>(args)...);
            RenderRegister::renderRegister->AddComponent(component.get(), context);

            return component;
        }

        static void DestroyComponent(IRenderComponent* component)
        {
            if (RenderRegister::renderRegister == nullptr)
                throw std::runtime_error("Render Register not Initialized");

            renderRegister->RemoveFromRegister(component);
        }
    };
} // namespace Beer::System
