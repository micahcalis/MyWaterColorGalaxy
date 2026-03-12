#pragma once

#include "System/Drawing/RenderRegister.hpp"
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

    Core::DrawCallPool GetDrawCallPool(const DrawRequest request);

} // namespace Beer::System
