#pragma once

#include "Core/Assets/AssetCache.hpp"
#include <memory>
#include <filesystem>

namespace Beer::Core
{
    template<typename T>
    class IAssetManager
    {
    protected:
        AssetCache<T> cache;

    public:
        virtual ~IAssetManager() = default;

        virtual std::shared_ptr<T> Load(const std::string& name, const std::filesystem::path& path) = 0;

        std::shared_ptr<T> Get(const std::string& name)
        {
            std::shared_ptr<T> asset = cache.Get(name);

            if (asset == nullptr)
            {
                const std::filesystem::path path = GetPath(name);
                asset = Load(name, path);
            }

            return asset;
        }

        void Clear()
        {
            cache.Clear();
        }

    protected:
        virtual const std::filesystem::path GetPath(const std::string& name) = 0;
    };
} // namespace Beer::Core
