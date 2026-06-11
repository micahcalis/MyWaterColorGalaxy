#include "System/Context/ContextHandler.hpp"
#include "System/Context/IContext.hpp"
#include <stdexcept>

namespace Beer::System
{
    void ContextHandler::LoadContext(const ContextType type)
    {
        std::shared_ptr<IContext> context = GetContextFromType(type);
        context->Load();
        worldContainer->AddContext(type, std::move(context));
    }

    void ContextHandler::DestroyContext(const ContextType type)
    {
        worldContainer->DestroyContext(type);
    }

    void ContextHandler::SetContextActive(const ContextType type, const bool active)
    {
        throw std::runtime_error("SetContextActive is not implemented in ContextHandler!");
    }

    bool ContextHandler::ContextExists(ContextType type) const
    {
        return worldContainer->ContextExists(type);
    }

    std::shared_ptr<IContext> ContextHandler::GetContextFromType(const ContextType type)
    {
        auto factory = contextFactories.find(type);

        if (factory == contextFactories.end())
            throw std::runtime_error("Context Type not defined in creation factory");

        return factory->second();
    }

    void ContextHandler::FlushOperations()
    {
        for (const auto& operation : queuedOperations)
        {
            operation();
        }

        queuedOperations.clear();
    }
} // namespace Beer::System
