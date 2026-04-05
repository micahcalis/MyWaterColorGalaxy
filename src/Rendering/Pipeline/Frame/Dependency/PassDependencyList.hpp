#pragma once

#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Shader/FragmentOutput.hpp"
#include "vulkan/vulkan.hpp"
#include <stdexcept>
#include <vector>

namespace Beer::Rendering
{
    class PassDependencyList
    {
    private:
        std::string passName;
        std::vector<PassDependency> dependencies;

    public:
        PassDependencyList(const std::string& passName)
            : passName(passName)
        {
        }

        [[nodiscard]] const std::string& GetPassName() const { return passName; }
        [[nodiscard]] const std::vector<PassDependency>& GetDependencies() const { return dependencies; }

        void AddDependency(PassDependency dependency)
        {
            dependencies.emplace_back(dependency);
        }

        void AddDependencies(std::vector<PassDependency> dependencies)
        {
            this->dependencies.append_range(dependencies);
        }

        FragmentOutput GetOutput()
        {
            FragmentOutput output{};

            for (const auto& dependency : dependencies)
            {
                if (dependency.GetAction() == ResourceAction::ColorWrite)
                {
                    vk::Format format = dependency.GetFormat();

                    if (format == vk::Format::eUndefined)
                    {
                        throw std::runtime_error("Undefined Format in Color Write Dependency");
                    }

                    output.ColorFormats.push_back(dependency.GetFormat());
                }

                if (dependency.GetAction() == ResourceAction::DepthWrite)
                {
                    output.WritesDepth = true;
                }
            }

            return output;
        }
    };
} // namespace Beer::Rendering
