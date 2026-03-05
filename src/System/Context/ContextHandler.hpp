#pragma once

#include "System/Context/ContextType.hpp"
#include "System/Context/WorldContainer.hpp"

namespace Beer::System
{
    class ContextHandler
    {
    private:
        WorldContainer* worldContainer;

    public:
        ContextHandler(WorldContainer* worldContainer)
            : worldContainer(worldContainer)
        {
        }

        void LoadContext(const ContextType type);
        void DestroyContext(const ContextType type);
        void SetContextActive(const ContextType type, const bool active);

    private:
        std::shared_ptr<IContext> GetContextFromType(const ContextType type);
    };
} // namespace Beer::System
