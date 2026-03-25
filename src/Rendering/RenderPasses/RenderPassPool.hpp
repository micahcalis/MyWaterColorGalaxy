#pragma once

#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <string>
#include <type_traits>

namespace Beer::Rendering
{
    class IRenderPass;

    using TypeID = const void*;

    template<typename T>
    TypeID GetTypeID()
    {
        static const char id{};
        return &id;
    }

    class RenderPassPool
    {
    private:
        struct PassEntry
        {
            std::unique_ptr<IRenderPass> pass;
            TypeID typeId;
        };

        std::unordered_map<std::string, PassEntry> cache;

    public:
        template<typename T, typename... Args>
        T* FetchRenderPass(const std::string& name, Args&&... args)
        {
            static_assert(std::is_base_of_v<IRenderPass, T>, "Type must inherit from IRenderPass");

            auto lookUp = GetRenderPass<T>(name);
            if (lookUp != nullptr)
            {
                return lookUp;
            }

            return CreateRenderPass<T>(name, std::forward<Args>(args)...);
        }

        template<typename T>
        T* GetRenderPass(const std::string& name)
        {
            auto it = cache.find(name);
            if (it != cache.end())
            {
                if (it->second.typeId == GetTypeID<T>())
                {
                    return static_cast<T*>(it->second.pass.get());
                } else
                {
                    throw std::runtime_error("Render Pass fetch not mathcing type");
                }
            }

            return nullptr;
        }

        void DestroyRenderPass(const std::string& name)
        {
            cache.erase(name);
        }

    private:
        template<typename T, typename... Args>
        T* CreateRenderPass(const std::string& name, Args&&... args)
        {
            auto newPass = std::make_unique<T>(std::forward<Args>(args)...);
            T* rawPtr = newPass.get();

            cache.emplace(name, PassEntry{std::move(newPass), GetTypeID<T>()});
            return rawPtr;
        }
    };
} // namespace Beer::Rendering
