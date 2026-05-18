#pragma once

#include <print>
#include <vector>
#include "Vendor/magic_enum/magic_enum.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    struct FragmentOutput
    {
    public:
        std::vector<vk::Format> ColorFormats;
        bool TestsDepth = false;

    public:
        bool operator==(const FragmentOutput& other) const
        {
            return ColorFormats == other.ColorFormats && TestsDepth == other.TestsDepth;
        }

        bool HasUndefined() const
        {
            for (const auto format : ColorFormats)
            {
                if (format == vk::Format::eUndefined)
                {
                    return true;
                }
            }

            return false;
        }

        void Print() const
        {
            std::println("Tests With Depth: {}", TestsDepth);

            for (const auto format : ColorFormats)
            {
                std::println("Color Format: {}", magic_enum::enum_name(format));
            }
        }
    };

    struct FormatHasher
    {
        std::size_t operator()(const FragmentOutput& key) const
        {
            std::size_t hash = 0;

            auto hashCombine = [&hash](std::size_t value) {
                hash ^= value + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            };

            for (auto format : key.ColorFormats)
            {
                hashCombine(std::hash<uint32_t>()(static_cast<uint32_t>(format)));
            }

            hashCombine(std::hash<uint32_t>()(static_cast<uint32_t>(key.TestsDepth)));

            return hash;
        }
    };

    enum class FragOutputType
    {
        Color,
        Depth
    };

    struct FragOutputPair
    {
    public:
        FragOutputType Type;
        vk::ColorComponentFlags ColorComponents;

        FragOutputPair(FragOutputType type, vk::ColorComponentFlags colorComponents)
            : Type(type), ColorComponents(colorComponents)
        {
        }
    };

    struct FragmentTemplate
    {
    public:
        std::vector<FragOutputPair> TemplateList;

    public:
        void Add(FragOutputType type, vk::ColorComponentFlags colorComponents)
        {
            TemplateList.emplace_back(type, colorComponents);
        }

        bool OutputCompatible(const FragmentOutput& output) const
        {
            uint32_t templateCount = GetColorAttachCount();
            uint32_t outputCount = static_cast<uint32_t>(output.ColorFormats.size());

            if (templateCount != outputCount)
            {
                std::println("COLOR MISMATCH! Shader Template expects: {}, but Render Pass Output provided: {}", templateCount, outputCount);
                return false;
            }

            if (HasDepthWrite() && output.TestsDepth == false)
            {
                std::println("DEPTH MISMATCH! Shader requires Depth, but Render Pass does not provide it.");
                return false;
            }

            if (output.HasUndefined())
            {
                std::println("UNDEFINED FORMAT DETECTED in Render Pass Output.");
                return false;
            }

            return true;
        }

        void Print() const
        {
            for (const auto& pair : TemplateList)
            {
                std::println("FragOutputType: {}", magic_enum::enum_name(pair.Type));
                std::println("Component Count: {}", static_cast<uint32_t>(pair.ColorComponents));
            }
        }

    private:
        uint32_t GetColorAttachCount() const
        {
            uint32_t count = 0;

            for (const auto& pair : TemplateList)
            {
                if (pair.Type == FragOutputType::Color)
                {
                    count++;
                }
            }

            return count;
        }

        bool HasDepthWrite() const
        {
            for (const auto& pair : TemplateList)
            {
                if (pair.Type == FragOutputType::Depth)
                {
                    return true;
                }
            }

            return false;
        }
    };
} // namespace Beer::Rendering
