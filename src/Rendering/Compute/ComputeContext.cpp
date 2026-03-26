#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Compute/ComputeShader.hpp"

namespace Beer::Rendering
{
    ComputeContext::ComputeContext(std::shared_ptr<ComputeShader> compute)
        : compute(compute)
    {
        InitializeBufferData(GetProperties());
    }

    ComputeContext::ComputeContext(const std::string& computeName)
    {
        compute = ComputeShader::Get(computeName);
        InitializeBufferData(GetProperties());
    }

    void ComputeContext::Update()
    {
        buffer->Update(*materialData);
    }
} // namespace Beer::Rendering
