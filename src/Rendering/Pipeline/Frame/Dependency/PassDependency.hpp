#pragma once

#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include <string>

namespace Beer::Rendering
{
    struct PassDependency
    {
    private:
        std::string resourceName;
        ResourceAction action;

    public:
        PassDependency(const std::string& resourceName, ResourceAction action)
            : resourceName(resourceName), action(action)
        {
        }

        [[nodiscard]] const std::string& GetResourceName() const { return resourceName; }
        [[nodiscard]] const ResourceAction GetAction() const { return action; }
    };
} // namespace Beer::Rendering
