#pragma once

#include "System/Context/ContextType.hpp"
#include "System/Context/WorldContainer.hpp"
#include "System/Delegates/Delegate.hpp"

namespace Beer::System
{
    class ContextHandler
    {
    private:
        WorldContainer* worldContainer;
        std::unordered_map<ContextType, Function<std::shared_ptr<IContext>>> contextFactories;

    public:
        ContextHandler(WorldContainer* worldContainer)
            : worldContainer(worldContainer)
        {
        }

        void RegisterContextFactory(ContextType type, Function<std::shared_ptr<IContext>> factory)
        {
            contextFactories[type] = factory;
        }

        void LoadContext(const ContextType type);
        void DestroyContext(const ContextType type);
        void SetContextActive(const ContextType type, const bool active);

    private:
        std::shared_ptr<IContext> GetContextFromType(const ContextType type);
    };
} // namespace Beer::System
