#pragma once

#include "CommandBuffer/RenderContext.hpp"
#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"
#include "Rendering/RenderPasses/RenderPassPool.hpp"
#include "Rendering/Shader/FragmentOutput.hpp"
#include <stdexcept>

namespace Beer::Rendering
{
    class IRenderPass
    {
    protected:
        std::string name;
        uint32_t event;

    public:
        virtual ~IRenderPass() = default;

        IRenderPass(std::string name, uint32_t event)
            : name(name), event(event)
        {
        }

        IRenderPass(std::string name, RenderPassEvent event)
            : name(name), event(static_cast<uint32_t>(event))
        {
        }

        const std::string& GetName() const { return name; }
        uint32_t GetEvent() const { return event; }

        virtual void OnRenderSetup(const RenderContext& context) = 0;
        virtual void Execute(CommandBuffer* commandBuffer, const RenderContext& context) = 0;
        virtual PassDependencyList GetDependencies() const = 0;

        virtual bool BlitsMainTarget() const
        {
            return false;
        }

    private:
        inline static RenderPassPool* renderPassPool = nullptr;

    public:
        template<typename T, typename... Args>
        static T* FetchFromRegister(const std::string& name, Args&&... args)
        {
            if (renderPassPool == nullptr)
                throw std::runtime_error("Render Pass Pool not initialized");

            return renderPassPool->FetchRenderPass<T>(name, std::forward<Args>(args)...);
        }

        static void SetPool(RenderPassPool* pool)
        {
            renderPassPool = pool;
        }
    };
} // namespace Beer::Rendering
