#pragma once

#include "System/Drawing/RenderRegister.hpp"
#include "Core/Application/Renderer/DrawCallPool.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "System/Components/UI/UIRenderComponent.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Context/ContextType.hpp"

namespace Beer::System
{
    RenderRegister::RenderRegister()
    {
        for (const ContextType context : AVAILABLE_CONTEXTS)
        {
            componentsMap[context];
        }
    }

    void RenderRegister::AddComponent(IRenderComponent* component, const ContextType context)
    {
        componentsMap[context].emplace_back(component);
    }

    void RenderRegister::RemoveFromRegister(IRenderComponent* component)
    {
        for (auto& [context, componentList] : componentsMap)
        {
            std::erase(componentList, component);
        }
    }

    void RenderRegister::Cleanup()
    {
        for (auto& [context, componentList] : componentsMap)
        {
            std::erase(componentList, nullptr);
        }
    }

    Core::DrawCallPool RenderRegister::GetDrawCallPool(const DrawRequest& request)
    {
        std::vector<IRenderComponent*> validatedComponents;
        validatedComponents.reserve(2048);

        for (const auto& [context, componentList] : componentsMap)
        {
            if (!request.ValidateContext(context))
                continue;

            for (IRenderComponent* component : componentList)
            {
                if (!component)
                    continue;

                if (IsRenderComponentValid(request, component))
                    validatedComponents.emplace_back(component);
            }
        }

        if (request.GetPass() == Rendering::ShaderPassType::UserInterface)
        {
            validatedComponents = SortUIComponents(validatedComponents);
        }

        return Core::DrawCallPool(request.GetCommandBuffer(),
            request.GetContext(),
            request.GetPass(),
            std::move(validatedComponents));
    }

    bool RenderRegister::IsRenderComponentValid(const DrawRequest& request, IRenderComponent* component)
    {
        return request.ValidateLayer(component->GetLayer())
            && component->HasPass(request.GetPass());
    }

    std::vector<IRenderComponent*> RenderRegister::SortUIComponents(const std::vector<IRenderComponent*>& validatedComponents)
    {
        std::vector<IRenderComponent*> uiComponents;
        uiComponents.reserve(validatedComponents.size());

        for (auto& component : validatedComponents)
        {
            if (component->GetType() == RenderCompType::Sprite)
            {
                uiComponents.push_back(component);
            }
        }

        std::sort(uiComponents.begin(),
            uiComponents.end(),
            [](const IRenderComponent* a, const IRenderComponent* b) -> bool {
                const UIRenderComponent* uiCompA = static_cast<const UIRenderComponent*>(a);
                const UIRenderComponent* uiCompB = static_cast<const UIRenderComponent*>(b);

                return uiCompA->GetDepth() < uiCompB->GetDepth();
            });

        return uiComponents;
    }

} // namespace Beer::System
