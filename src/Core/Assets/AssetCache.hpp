#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace Beer::Core
{
    template<typename T>
    class AssetCache
    {
    private:
        std::unordered_map<std::string, std::shared_ptr<T>> cache;

    public:
        std::shared_ptr<T> Get(const std::string& name);
        void Add(const std::string& name, std::shared_ptr<T> asset);
        void Clear();

    private:
        bool Contains(const std::string& name) const;
    };
} // namespace Beer::Core

namespace Beer::Core
{
    template<typename T>
    std::shared_ptr<T> AssetCache<T>::Get(const std::string& name)
    {
        if (Contains(name))
            return cache.at(name);

        return nullptr;
    }

    template<typename T>
    void AssetCache<T>::Add(const std::string& name, std::shared_ptr<T> asset)
    {
        cache[name] = std::move(asset);
    }

    template<typename T>
    void AssetCache<T>::Clear()
    {
        cache.clear();
    }

    template<typename T>
    bool AssetCache<T>::Contains(const std::string& name) const
    {
        return cache.contains(name);
    }
} // namespace Beer::Core

