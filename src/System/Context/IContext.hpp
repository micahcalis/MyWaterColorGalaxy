#pragma once

#include "System/Context/ContextType.hpp"
#include <System/ECS/Registry.hpp>

namespace Beer::System
{
    class WorldContainer;
}

namespace Beer::System
{
    class IContext
    {
    protected:
        Registry registry;
        bool enabled = true;

    public:
        virtual ~IContext() = default;
        Registry& GetRegistry() { return registry; }
        virtual void Load() = 0;

        virtual void Update()
        {
            if (!enabled)
                return;
        }

        void SetActive(bool enabled)
        {
            this->enabled = enabled;
        }

    private:
        inline static WorldContainer* worldContainer = nullptr;

    public:
        static void SetWorldContainer(WorldContainer* worldContainer) { IContext::worldContainer = worldContainer; }
        static Registry& GetRegistry(ContextType type);
    };
} // namespace Beer::System
