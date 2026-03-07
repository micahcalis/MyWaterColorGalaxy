#include "System/Context/WorldContainer.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Context/IContext.hpp"
#include <memory>
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

    std::shared_ptr<IContext> WorldContainer::GetContext(const ContextType type) const
    {
        if (!ContextExists(type))
            return nullptr;

        return contextMap.at(type);
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
