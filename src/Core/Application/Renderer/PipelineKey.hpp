#pragma once

#include <vulkan/vulkan.h>
#include <string>

namespace Beer::Core
{
    struct PipelineKey
    {
    public:
        std::string PipelineName;
        uint64_t PipelineHash;

    public:
        PipelineKey(const std::string& name);
        bool operator==(const PipelineKey& other) const;
    };
} // namespace Beer::Core

namespace std
{
    template<>
    struct hash<Beer::Core::PipelineKey>
    {
        std::size_t operator()(const Beer::Core::PipelineKey& k) const
        {
            return k.PipelineHash;
        }
    };
} // namespace std
