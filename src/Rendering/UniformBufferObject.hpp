#include <glm/glm.hpp>

namespace Beer::Rendering
{
    struct UniformBufferObject
    {
        glm::mat4 objToWorld;
        glm::mat4 worldToView;
        glm::mat4 viewToClip;
    };
} // namespace Beer::Rendering
