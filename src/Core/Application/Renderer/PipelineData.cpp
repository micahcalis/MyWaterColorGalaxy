#include "Core/Application/Renderer/PipelineData.hpp"

namespace Beer::Core
{
    std::string PipelineData::GetVertexName() const { return VERT + ShaderName; }
    std::string PipelineData::GetFragmentName() const { return FRAG + ShaderName; }
} // namespace Beer::Core
