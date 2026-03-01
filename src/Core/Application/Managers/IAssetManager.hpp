#pragma once

#include "Core/Assets/AssetCache.hpp"
#include <memory>
#include <filesystem>
#include <stdexcept>

namespace Beer::Core
{
    template<typename T>
    class IAssetManager
    {
    protected:
        AssetCache<T> cache;

    public:
        virtual ~IAssetManager() = default;

        virtual std::shared_ptr<T> Load(const std::filesystem::path& path) = 0;

        std::shared_ptr<T> Get(const std::string& name)
        {
            std::shared_ptr<T> asset = cache.Get(name);

            if (asset == nullptr)
            {
                const std::filesystem::path path = GetPath(name);
                asset = Load(path);

                if (asset == nullptr)
                    throw std::runtime_error("Failed to load asset: nullptr");

                cache.Add(name, asset);
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
