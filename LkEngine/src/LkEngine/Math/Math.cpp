#include "LKpch.h"
#include "LkEngine/Math/Math.h"


namespace LkEngine::Math {

    glm::vec3 Scale(glm::vec3& vec, float scale_factor)
    {
        return (vec * scale_factor) / glm::length(vec);
    }


}