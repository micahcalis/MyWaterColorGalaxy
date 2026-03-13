#include "System/Context/IContext.hpp"
#include "System/Context/WorldContainer.hpp"

namespace Beer::System
{
    Registry* IContext::GetRegistry(ContextType type)
    {
        return worldContainer->GetContext(type)->GetRegistry();
    }
} // namespace Beer::System
