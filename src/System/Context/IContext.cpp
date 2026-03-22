#include "System/Context/IContext.hpp"
#include "System/Context/WorldContainer.hpp"

namespace Beer::System
{
    Registry* IContext::GetRegistry(ContextType type)
    {
        return worldContainer->GetContext(type)->GetRegistry();
    }

    std::vector<Rendering::IRenderPass*> IContext::GetActivePasses()
    {
        std::vector<IContext*> activeContexts = worldContainer->GetActiveContexts();
        std::vector<Rendering::IRenderPass*> passes;

        for (auto& context : activeContexts)
        {
            passes.append_range(context->GetRenderPasses());
        }

        return passes;
    }
} // namespace Beer::System
