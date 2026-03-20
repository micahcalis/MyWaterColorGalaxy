#pragma once

#include "vulkan/vulkan.hpp"
namespace Beer::Rendering
{
    struct ResetOperator
    {
    public:
        vk::AttachmentLoadOp LoadOp;
        vk::AttachmentStoreOp StoreOp;
        vk::ClearValue ClearValue;

    public:
        ResetOperator(vk::AttachmentLoadOp loadOp = vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp storeOp = vk::AttachmentStoreOp::eStore,
            vk::ClearValue clearValue = vk::ClearValue{})
            : LoadOp(loadOp), StoreOp(storeOp), ClearValue(clearValue)
        {
        }

        static ResetOperator ClearColor(std::array<float, 4> color = {0.0f, 0.0f, 0.0f, 1.0f})
        {
            vk::ClearValue cv;
            cv.color = vk::ClearColorValue(color);
            return ResetOperator(vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, cv);
        }

        static ResetOperator ClearDepth(float depth = 1.0f, uint32_t stencil = 0)
        {
            vk::ClearValue cv;
            cv.depthStencil = vk::ClearDepthStencilValue(depth, stencil);
            return ResetOperator(vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, cv);
        }

        static ResetOperator DontCare(vk::AttachmentStoreOp storeOp = vk::AttachmentStoreOp::eStore)
        {
            return ResetOperator(vk::AttachmentLoadOp::eDontCare, storeOp);
        }
    };
} // namespace Beer::Rendering
