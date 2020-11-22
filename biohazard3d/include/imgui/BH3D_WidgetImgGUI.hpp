
#pragma once

#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "BH3D_Camera.hpp"

#include <glm/gtc/constants.hpp>

namespace bh3d
{
    namespace widget3d
    {
        constexpr float WORLD_BORDER = 150.0f;
        constexpr glm::vec4 COLOR_TITLE_WIDGET = { 240.0f/255.0f, 160.0f/255.0f, 50.0f/255.0f, 1.0f };

        inline ImVec4 glm2imvec(const glm::vec4 & vec) {
            return ImVec4(vec.x, vec.y, vec.z, vec.w );
        }

        template<class T>
        void epsiloner(T & value) {
            static float epsilon = glm::sqrt(glm::epsilon<float>());
            value = std::max(value, epsilon);
        }

        bool ProjectionMatrix(CameraProjectionInfos & cameraProjectInfo, float z_min = 0.0f, float z_max = WORLD_BORDER);

        bool LookAtMatrix(CameraLookAtInfos & cameraLookAtInfos, float v_min = -WORLD_BORDER, float v_max = WORLD_BORDER);
    
         bool CameraFeatures(Camera & camera, float v_min = -WORLD_BORDER, float v_max = WORLD_BORDER);
    }
}