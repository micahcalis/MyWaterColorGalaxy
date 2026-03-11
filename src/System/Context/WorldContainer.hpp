#pragma once

#include "System/Context/ContextType.hpp"
#include "System/Context/IContext.hpp"
#include <unordered_map>
#include <memory>

namespace Beer::System
{
    class WorldContainer
    {
    private:
        std::unordered_map<ContextType, std::shared_ptr<IContext>> contextMap;

    public:
        void AddContext(const ContextType type, std::shared_ptr<IContext>);
        IContext* GetContext(const ContextType type) const;
        void UpdateContexts();
        void DestroyContext(const ContextType type);

    private:
        void CleanupDirty();
        bool ContextExists(const ContextType type) const { return contextMap.contains(type); }
    };
} // namespace Beer::System
