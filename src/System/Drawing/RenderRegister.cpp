#pragma once

#include "System/Drawing/RenderRegister.hpp"
#include "Core/Application/Renderer/DrawCallPool.hpp"
#include "System/Components/General/IRenderComponent.hpp"
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

        return Core::DrawCallPool(request.GetCommandBuffer(),
            request.GetPass(),
            std::move(validatedComponents));
    }

    bool RenderRegister::IsRenderComponentValid(const DrawRequest& request, IRenderComponent* component)
    {
        return request.ValidateLayer(component->GetLayer())
            && component->HasPass(request.GetPass());
    }

} // namespace Beer::System
