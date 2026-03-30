#pragma once

#include "Rendering/Compute/ComputeShader.hpp"
#include "Rendering/Material/IReflectedContext.hpp"

namespace Beer::Rendering
{
    class ComputeContext : public IReflectedContext
    {
    private:
        std::shared_ptr<ComputeShader> compute = nullptr;

    public:
        ComputeContext(std::shared_ptr<ComputeShader> compute);
        ComputeContext(const std::string& computeName);
        const ComputeShader* GetCompute() const { return compute.get(); }

        void Update() override;

    protected:
        MaterialProperties* GetProperties() override { return compute->GetProperties(); }
    };
} // namespace Beer::Rendering
