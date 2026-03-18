#pragma once

#include "Rendering/Pipeline/Frame/Dependency/ResourceActionMask.hpp"
#include <string>
namespace Beer::Rendering
{
    struct PassDependency
    {
    private:
        std::string resourceName;
        ResourceActionMask actionMask;

    public:
        PassDependency(std::string& resourceName, ResourceActionMask actionMask)
            : resourceName(resourceName), actionMask(actionMask)
        {
        }

        [[nodiscard]] const std::string& GetResourceName() const { return resourceName; }
        [[nodiscard]] const ResourceActionMask GetActionMask() const { return actionMask; }
    };
} // namespace Beer::Rendering
