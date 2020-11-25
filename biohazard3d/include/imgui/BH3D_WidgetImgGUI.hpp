
#pragma once

#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "BH3D_Camera.hpp"

#include <glm/gtc/constants.hpp>

#define BH3D_IMGUI_LOCK_ID() ::bh3d::widget3d::ImGuiLockID __ImGuiLockID(__LINE__)
#define BH3D_IMGUI_LOCK_ID_(name) ::bh3d::widget3d::ImGuiLockID name##__ImGuiLockID(__LINE__)

namespace bh3d
{
    namespace widget3d
    {
        constexpr float WORLD_BORDER = 150.0f;
        constexpr glm::vec4 COLOR_TITLE_WIDGET = { 240.0f/255.0f, 160.0f/255.0f, 50.0f/255.0f, 1.0f };

        inline ImVec4 glm2imvec(const glm::vec4 & vec) {
            return ImVec4(vec.x, vec.y, vec.z, vec.w );
        }


        struct ImGuiLockID
        {
            template <class T>
            ImGuiLockID(T &&id)
            {
                ImGui::PushID(std::forward<T>(id));
            }
            ~ImGuiLockID()
            {
                ImGui::PopID();
            }
        };

        template <class T>
        class ResetSaveRestore
        {
        public:
            ResetSaveRestore(const T &instance) : m_saver(instance),
                                                  m_reset(instance)
            {
            }

            T m_saver;
            T m_reset;

            bool Widget(T &instance)
            {
                bool updated = false;
                if (ImGui::Button("Reset"))
                {
                    instance = m_reset;
                    updated = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Save"))
                {
                    m_saver = instance;
                }
                ImGui::SameLine();
                if (ImGui::Button("Restore"))
                {
                    instance = m_saver;
                    updated = true;
                }
                return updated;
            }
        };

        template<class T>
        void epsiloner(T & value) {
            static float epsilon = glm::sqrt(glm::epsilon<float>());
            value = std::max(value, epsilon);
        }

        bool ProjectionMatrix(CameraProjectionInfos & cameraProjectInfo, float z_min = 0.0f, float z_max = WORLD_BORDER);

        bool LookAtMatrix(CameraLookAtInfos & cameraLookAtInfos, float v_min = -WORLD_BORDER, float v_max = WORLD_BORDER);
    
        bool CameraFeatures(Camera & camera, float v_min = -WORLD_BORDER, float v_max = WORLD_BORDER);

        bool CameraMainFeatures(Camera & camera, float v_min = -WORLD_BORDER, float v_max = WORLD_BORDER);

    }
}