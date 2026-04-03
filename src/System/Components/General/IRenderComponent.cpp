#include "System/Components/General/IRenderComponent.hpp"
#include "System/Drawing/RenderRegister.hpp"

namespace Beer::System
{
    IRenderComponent::~IRenderComponent()
    {
        RenderRegister::DestroyComponent(this);
    }
} // namespace Beer::System
