#include "Core/Application/Renderer/PipelineData.hpp"

namespace Beer::Core
{
    std::string PipelineData::GetVertexName() const { return ShaderName + std::string(VERT); }
    std::string PipelineData::GetFragmentName() const { return ShaderName + std::string(FRAG); }
} // namespace Beer::Core
