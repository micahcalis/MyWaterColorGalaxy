#pragma once

#include "IContext.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Context/WorldContainer.hpp"
#include "System/Delegates/Delegate.hpp"
#include <stdexcept>
#include <type_traits>

namespace Beer::System
{
    class ContextHandler
    {
    private:
        WorldContainer* worldContainer;
        std::unordered_map<ContextType, Function<std::shared_ptr<IContext>>> contextFactories;
        std::vector<Function<void>> queuedOperations;

    public:
        ContextHandler(WorldContainer* worldContainer)
            : worldContainer(worldContainer)
        {
        }

        void Update()
        {
            FlushOperations();
        }

        void RegisterContextFactory(ContextType type, Function<std::shared_ptr<IContext>> factory)
        {
            contextFactories[type] = factory;
        }

        void LoadContext(const ContextType type);
        void DestroyContext(const ContextType type);
        void SetContextActive(const ContextType type, const bool active);

        template<typename T>
        T* GetContext(const ContextType type)
        {
            static_assert(std::is_base_of<IContext, T>::value, "type must inherit from IContext");

            IContext* context = worldContainer->GetContext(type);

            if (context == nullptr)
            {
                throw std::runtime_error(std::format("No Context Found with type: {}!", magic_enum::enum_name(type)));
            }

            return static_cast<T*>(context);
        }

        void QueueOperation(Function<void> operation)
        {
            if (operation != nullptr)
            {
                queuedOperations.push_back(operation);
            }
        }

    private:
        std::shared_ptr<IContext> GetContextFromType(const ContextType type);
        void FlushOperations();
    };
} // namespace Beer::System
