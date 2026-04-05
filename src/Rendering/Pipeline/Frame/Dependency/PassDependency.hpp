#pragma once

#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "ResetOperator.hpp"
#include "vulkan/vulkan.hpp"
#include <string>

namespace Beer::Rendering
{
    struct PassDependency
    {
    private:
        std::string resourceName;
        ResourceAction action;
        ResetOperator resetOperator;
        vk::Format format;

    public:
        PassDependency(const std::string& resourceName,
            ResourceAction action,
            ResetOperator resetOperator = ResetOperator(),
            vk::Format format = vk::Format::eUndefined)
            : resourceName(resourceName), action(action), resetOperator(resetOperator), format(format)
        {
        }

        [[nodiscard]] const std::string& GetResourceName() const { return resourceName; }
        [[nodiscard]] const ResourceAction GetAction() const { return action; }
        [[nodiscard]] const ResetOperator GetResetOperator() const { return resetOperator; }
        [[nodiscard]] const vk::Format GetFormat() const { return format; }
    };
} // namespace Beer::Rendering
