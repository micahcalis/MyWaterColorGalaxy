#include "System/Context/ContextHandler.hpp"
#include "System/Context/IContext.hpp"
#include "System/Galaxy/GalaxyContext.hpp"

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
    }

    std::shared_ptr<IContext> ContextHandler::GetContextFromType(const ContextType type)
    {
        auto factory = contextFactories.find(type);

        if (factory == contextFactories.end())
            throw std::runtime_error("Context Type not defined in creation factory");

        return factory->second();
    }
} // namespace Beer::System
