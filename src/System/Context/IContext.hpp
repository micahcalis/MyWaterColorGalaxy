#pragma once

#include "Rendering/Pipeline/IRenderPass.hpp"
#include "System/Context/ContextType.hpp"
#include <System/Components/Registry/Registry.hpp>

namespace Beer::System
{
    class WorldContainer;
}

namespace Beer::System
{
    class IContext
    {
    protected:
        Registry registry{};
        bool enabled = true;

    public:
        virtual ~IContext() = default;
        Registry* GetRegistry() { return &registry; }
        virtual void Load() = 0;

        virtual void Update()
        {
            if (!enabled)
                return;
        }

        virtual std::vector<Rendering::IRenderPass*> GetRenderPasses() = 0;

        void SetActive(bool enabled)
        {
            this->enabled = enabled;
        }

        bool IsActive() const
        {
            return enabled;
        }

    private:
        inline static WorldContainer* worldContainer = nullptr;

    public:
        static void SetWorldContainer(WorldContainer* worldContainer) { IContext::worldContainer = worldContainer; }
        static Registry* GetRegistry(ContextType type);
        static std::vector<Rendering::IRenderPass*> GetActivePasses();
    };
} // namespace Beer::System
