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
        worldContainer->GetContext(type).reset();
    }

    void ContextHandler::SetContextActive(const ContextType type, const bool active)
    {
    }

    std::shared_ptr<IContext> ContextHandler::GetContextFromType(const ContextType type)
    {
        std::shared_ptr<IContext> context = nullptr;

        switch (type)
        {
        case ContextType::Galaxy:
            context = std::make_shared<GalaxyContext>();
            break;
        }

        if (context == nullptr)
        {
            throw std::runtime_error("Context Type not defined in creation pipeline");
        }

        return context;
    }
} // namespace Beer::System
