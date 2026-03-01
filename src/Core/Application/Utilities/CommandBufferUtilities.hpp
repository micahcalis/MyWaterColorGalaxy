#pragma once

#include "Core/Application/Renderer/FrameResource.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"

namespace Beer::Core
{
    class CommandBufferUtilities
    {
    public:
        static void TransitionImageLayout(vk::CommandBuffer& commandBuffer,
            vk::Image image,
            vk::ImageLayout oldLayout,
            vk::ImageLayout newLayout,
            vk::AccessFlags2 srcAccessMask,
            vk::AccessFlags2 dstAccessMask,
            vk::PipelineStageFlags2 srcStageMask,
            vk::PipelineStageFlags2 dstStageMask,
            vk::ImageAspectFlags imageAspectFlags);

        static void TransitionImageLayout(vk::Image image,
            vk::ImageLayout oldLayout,
            vk::ImageLayout newLayout,
            const FrameResource& frameResource,
            const Device& device);

        static inline std::vector<vk::Buffer> ConvertToVkBuffers(const std::vector<Rendering::Buffer*>& customBuffers);

        static void BindMesh(vk::CommandBuffer commandBuffer,
            const Rendering::Mesh* mesh,
            const Rendering::MeshBufferOrder& bufferOrder,
            bool& canIndex);

        static void BindShaderPass(vk::CommandBuffer commandBuffer,
            const Rendering::ShaderPass* shaderPass);

        static void DrawCall(vk::CommandBuffer commandBuffer,
            const vk::raii::Pipeline& pipeline,
            const VkBuffer& vertexBuffer);

        static void DrawIndexedCall(vk::CommandBuffer commandBuffer,
            const vk::raii::Pipeline& pipeline,
            const VkBuffer& vertexBuffer,
            const VkBuffer& indexBuffer,
            const uint32_t indexCount);

        static void DrawMesh(vk::CommandBuffer commandBuffer,
            const Rendering::Mesh* mesh,
            const Rendering::Shader* shader,
            const Rendering::ShaderPassType pass);

        static vk::raii::CommandBuffer BeginSingleTimeCommands(const FrameResource& frameResource,
            const Device& device);

        static void EndSingleTimeCommands(vk::raii::CommandBuffer& commandBuffer, const Device& device);

        static void CopyBufferToImage(const Rendering::Buffer& buffer,
            vk::Image image,
            uint32_t width,
            uint32_t height,
            const FrameResource& frameResource,
            const Device& device);
    };
} // namespace Beer::Core
