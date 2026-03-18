#pragma once

#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include <vector>

namespace Beer::Rendering
{
    class PassDependencyList
    {
    private:
        std::string passName;
        std::vector<PassDependency> dependencies;

    public:
        PassDependencyList(std::string& passName)
            : passName(passName)
        {
        }

        [[nodiscard]] const std::string& GetPassName() const { return passName; }
        [[nodiscard]] const std::vector<PassDependency>& GetDependencies() const { return dependencies; }

        void AddDependency(PassDependency dependency)
        {
            dependencies.emplace_back(dependency);
        }
    };
} // namespace Beer::Rendering
