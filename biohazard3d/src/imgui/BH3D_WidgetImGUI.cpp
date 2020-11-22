#include "BH3D_WidgetImgGUI.hpp"
#include "BH3D_imgui_widgets.hpp"

#define LOCK_IMGUI_ID() LockImGuiID lockImGui(__LINE__)

namespace bh3d
{
    namespace widget3d
    {

        struct LockImGuiID
        {
            template<class T>
            LockImGuiID(T && id) {
                ImGui::PushID(std::forward<T>(id));
            }
            ~LockImGuiID(){
                 ImGui::PopID();
            }
        };

        template<class T>
        class ResetSaveRestore
        {
        public:
            ResetSaveRestore(const T & instance) : 
                m_saver(instance),
                m_reset(instance)
            {   }

            T m_saver;
            T m_reset;

            bool Widget(T & instance)
            {
                bool updated = false;
                if(ImGui::Button("Reset"))
                {
                    instance = m_reset;
                    updated = true;
                }
                ImGui::SameLine();
                if(ImGui::Button("Save"))
                {
                    m_saver = instance;
                }
                ImGui::SameLine();
                if(ImGui::Button("Restore"))
                {
                    instance = m_saver;
                    updated = true;
                }         
                return updated;
            }
        };


        bool ProjectionMatrix(CameraProjectionInfos & cameraProjectInfo, float z_min, float z_max)
        {
            assert(z_min >= 0.0f);
            assert(z_max >= z_min);

            LOCK_IMGUI_ID();

            static auto RSR = ResetSaveRestore(cameraProjectInfo);

            float PI = glm::pi<float>()*2;
            ImGui::TextColored(glm2imvec(COLOR_TITLE_WIDGET), "Projection matrix features: ");
            bool updated = false;
            
            if(ImGui::SliderFloat("Angle FOV (radian)", &cameraProjectInfo.m_angle_fov, 0.0f, PI, "%.3f")) {
                epsiloner(cameraProjectInfo.m_angle_fov);
                updated = true;
            }

            if(ImGui::SliderFloat("Z Near (closest plan)", &cameraProjectInfo.m_zNear, z_min, cameraProjectInfo.m_zFar, "%.3f")) {
                epsiloner(cameraProjectInfo.m_zNear);
                updated = true;
            }

            updated |= ImGui::SliderFloat("Z Far (furthest plan)", &cameraProjectInfo.m_zFar, cameraProjectInfo.m_zNear, std::max(z_max, cameraProjectInfo.m_zFar), "%.3f");
    
            updated |= RSR.Widget(cameraProjectInfo);

            return updated;
        }

        bool LookAtMatrix(CameraLookAtInfos & cameraLookAtInfos, float v_min, float v_max)
        {
                assert(v_max >= v_min);

                LOCK_IMGUI_ID();

                static auto RSR = ResetSaveRestore(cameraLookAtInfos);

                ImGui::TextColored(glm2imvec(COLOR_TITLE_WIDGET), "Modelview/Camera matrix features: ");
                bool updated = ImGui::SliderFloat3("Position", glm::value_ptr(cameraLookAtInfos.m_position), v_min, v_max);
                if(ImGui::SliderFloat3("Direction", glm::value_ptr(cameraLookAtInfos.m_direction), -1.0f, 1.0f))
                {
                    cameraLookAtInfos.m_direction = glm::normalize(cameraLookAtInfos.m_direction);
                    updated = true;
                }

                if(ImGui::SliderFloat3("Up", glm::value_ptr(cameraLookAtInfos.m_up), -1.0f, 1.0f))
                {
                        cameraLookAtInfos.m_up = glm::normalize(cameraLookAtInfos.m_up);
                        updated = true;
                }

                updated |= ImGui::SliderFloat3("Target", glm::value_ptr(cameraLookAtInfos.m_target), v_min, v_max);
                updated |= ImGui::SliderFloat("Zoom", &cameraLookAtInfos.m_zoom, 0.0f, v_max);

                updated |= RSR.Widget(cameraLookAtInfos);


                return updated;
        }


        bool CameraFeatures(Camera & camera, float v_min, float v_max)
        {
            assert(v_max >= v_min);

            LOCK_IMGUI_ID();

            static auto RSR = ResetSaveRestore(camera);

            ImGui::TextColored(glm2imvec(COLOR_TITLE_WIDGET), "Camera features: ");

            bool updated = ImGui::Checkbox("Target as direction: ", &camera.m_use_target);
            updated |= ImGui::SliderFloat("Mov. speed",         &camera.m_movement_speed, v_min, v_max, "%.3f");
            updated |= ImGui::SliderFloat("Mouse speed",        &camera.m_mouse_speed, v_min*0.001f, v_max*0.001f, "%.3f");
            updated |= ImGui::SliderFloat("Zoom speed",         &camera.m_zoom_speed, v_min, v_max, "%.3f");
            updated |= ImGui::SliderFloat("Zoom max",           &camera.m_zoom_max, std::max(camera.m_zoom_min, v_min), v_max, "%.3f");
            updated |= ImGui::SliderFloat("Zoom min",           &camera.m_zoom_min, v_min, std::min(camera.m_zoom_max, v_max), "%.3f");
            updated |= ImGui::SliderFloat("Default zoom",       &camera.m_default_zoom, v_min, v_max, "%.3f");
            updated |= ImGui::SliderFloat("Elapse time (fps)",  &camera.m_fps_elapse_time, v_min, v_max, "%.3f");
            
            updated |= RSR.Widget(camera);
            
            return updated;
        }

    }

}