#include "System/Context/WorldContainer.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Context/IContext.hpp"
#include <memory>
#include <print>
#include <stdexcept>

namespace Beer::System
{
    void WorldContainer::AddContext(const ContextType type, std::shared_ptr<IContext> context)
    {
        if (contextMap.contains(type))
        {
            throw std::runtime_error("Trying to add a context that exists");
        }

        contextMap[type] = context;
    }

    IContext* WorldContainer::GetContext(const ContextType type) const
    {
        if (!ContextExists(type))
            return nullptr;

        return contextMap.at(type).get();
    }

    std::vector<IContext*> WorldContainer::GetActiveContexts() const
    {
        std::vector<IContext*> activeContexts;
        activeContexts.reserve(contextMap.size());

        for (const auto& [key, context] : contextMap)
        {
            if (context->IsActive())
            {
                activeContexts.push_back(context.get());
            }
        }

        return activeContexts;
    }

    void WorldContainer::DestroyContext(const ContextType type)
    {
        contextMap.erase(type);
    }

    void WorldContainer::UpdateContexts()
    {
        CleanupDirty();

        for (auto context : contextMap)
        {
            context.second->Update();
        }
    }

    void WorldContainer::CleanupDirty()
    {
        std::erase_if(contextMap, [](const auto& pair) {
            return pair.second == nullptr;
        });
    }
} // namespace Beer::System
