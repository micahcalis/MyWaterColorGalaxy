#include "Core/Application/Renderer/PipelineKey.hpp"
#include <functional>

namespace Beer::Core
{
    PipelineKey::PipelineKey(const std::string& name)
    {
        PipelineName = name;
        PipelineHash = std::hash<std::string>{}(name);
    }

    bool PipelineKey::operator==(const PipelineKey& other) const
    {
        return PipelineHash == other.PipelineHash;
    }
} // namespace Beer::Core
