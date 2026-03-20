#pragma once

#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "ResetOperator.hpp"
#include <string>

namespace Beer::Rendering
{
    struct PassDependency
    {
    private:
        std::string resourceName;
        ResourceAction action;
        ResetOperator resetOperator;

    public:
        PassDependency(const std::string& resourceName,
            ResourceAction action,
            ResetOperator resetOperator = ResetOperator())
            : resourceName(resourceName), action(action), resetOperator(resetOperator)
        {
        }

        [[nodiscard]] const std::string& GetResourceName() const { return resourceName; }
        [[nodiscard]] const ResourceAction GetAction() const { return action; }
        [[nodiscard]] const ResetOperator GetResetOperator() const { return resetOperator; }
    };
} // namespace Beer::Rendering
