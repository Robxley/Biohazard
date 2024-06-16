#include "BH3D_WidgetImgGUI.hpp"
#include "BH3D_imgui_widgets.hpp"
#include <IconsFontAwesome6.h>

namespace bh3d
{
	namespace widget3d
	{
		bool ProjectionMatrix(CameraProjectionInfos& cameraProjectInfo, float z_min, float z_max)
		{
			assert(z_min >= 0.0f);
			assert(z_max >= z_min);

			BH3D_IMGUI_LOCK_ID();

			static auto RSR = ResetSaveRestore(cameraProjectInfo);

			ImGui::TextColored(glm2imvec(COLOR_TITLE_WIDGET), "Projection matrix features: ");
			bool updated = false;

			if (ImGui::SliderFloat("Angle FOV (radian)", &cameraProjectInfo.m_angle_fov, 0.0f, glm::pi<float>(), "%.3f"))
			{
				epsiloner(cameraProjectInfo.m_angle_fov);
				updated = true;
			}

			if (ImGui::SliderFloat("Z Near (closest plan)", &cameraProjectInfo.m_zNear, z_min, cameraProjectInfo.m_zFar, "%.3f"))
			{
				epsiloner(cameraProjectInfo.m_zNear);
				updated = true;
			}

			updated |= ImGui::SliderFloat("Z Far (furthest plan)", &cameraProjectInfo.m_zFar, cameraProjectInfo.m_zNear, std::max(z_max, cameraProjectInfo.m_zFar), "%.3f");

			updated |= RSR.Widget(cameraProjectInfo);

			return updated;
		}

		bool LookAtMatrix(CameraLookAtInfos& cameraLookAtInfos, float v_min, float v_max)
		{
			assert(v_max >= v_min);

			BH3D_IMGUI_LOCK_ID();

			static auto RSR = ResetSaveRestore(cameraLookAtInfos);

			ImGui::TextColored(glm2imvec(COLOR_TITLE_WIDGET), "Modelview/Camera matrix features: ");
			bool updated = ImGui::SliderFloat3("Position", glm::value_ptr(cameraLookAtInfos.m_position), v_min, v_max);
			if (ImGui::SliderFloat3("Direction", glm::value_ptr(cameraLookAtInfos.m_direction), -1.0f, 1.0f))
			{
				cameraLookAtInfos.m_direction = glm::normalize(cameraLookAtInfos.m_direction);
				updated = true;
			}

			if (ImGui::SliderFloat3("Up", glm::value_ptr(cameraLookAtInfos.m_up), -1.0f, 1.0f))
			{
				cameraLookAtInfos.m_up = glm::normalize(cameraLookAtInfos.m_up);
				updated = true;
			}

			updated |= ImGui::SliderFloat3("Target", glm::value_ptr(cameraLookAtInfos.m_target), v_min, v_max);

			updated |= RSR.Widget(cameraLookAtInfos);

			return updated;
		}

		bool CameraFeatures(Camera& camera, float v_min, float v_max)
		{
			assert(v_max >= v_min);

			BH3D_IMGUI_LOCK_ID();

			static auto RSR = ResetSaveRestore(camera);

			ImGui::TextColored(glm2imvec(COLOR_TITLE_WIDGET), "Camera features: ");

			bool updated = ImGui::Checkbox("Target as direction: ", &camera.m_use_target);
			updated |= ImGui::SliderFloat("Mov. speed", &camera.m_speed_movement, v_min, v_max, "%.3f");
			updated |= ImGui::SliderFloat("Mouse speed", &camera.m_speed_mouse, v_min * 0.001f, v_max * 0.001f, "%.3f");
			updated |= ImGui::SliderFloat("Zoom speed", &camera.m_zoom_speed, v_min, v_max, "%.3f");
			updated |= ImGui::SliderFloat("Zoom max", &camera.m_zoom_max, std::max(camera.m_zoom_min, v_min), v_max, "%.3f");
			updated |= ImGui::SliderFloat("Zoom min", &camera.m_zoom_min, v_min, std::min(camera.m_zoom_max, v_max), "%.3f");
			updated |= ImGui::SliderFloat("Default zoom", &camera.m_zoom_default, v_min, v_max, "%.3f");
			updated |= ImGui::SliderFloat("Elapse time (fps)", &camera.m_fps_elapse_time, v_min, v_max, "%.3f");

			updated |= RSR.Widget(camera);

			return updated;
		}

		bool CameraMainFeatures(Camera& camera, float v_min, float v_max)
		{
			assert(v_max >= v_min);
			BH3D_IMGUI_LOCK_ID();
			static auto RSR = ResetSaveRestore(camera);
			ImGui::TextColored(glm2imvec(COLOR_TITLE_WIDGET), "Camera features: ");
			bool updated = ImGui::SliderFloat3("Position", glm::value_ptr(camera.m_position), v_min, v_max);
			if (ImGui::SliderFloat3("Direction", glm::value_ptr(camera.m_direction), -1.0f, 1.0f))
			{
				camera.m_direction = glm::normalize(camera.m_direction);
				updated = true;
			}

			if (ImGui::SliderFloat("Zoom (Angle FOV)", &camera.m_angle_fov, 0.0f, glm::pi<float>(), "%.3f"))
			{
				epsiloner(camera.m_angle_fov);
				updated = true;
			}

			updated |= ImGui::SliderFloat3("Target", glm::value_ptr(camera.m_target), v_min, v_max);
			ImGui::SameLine();
			updated |= ImGui::Checkbox("###Target", &camera.m_use_target);

			updated |= RSR.Widget(camera);
			return updated;
		}

		bool CameraZoomFov(Camera& camera, float zoom_step)
		{
			BH3D_IMGUI_LOCK_ID();
			static auto RSR = ResetSaveRestore(camera);
			ImGui::TextColored(glm2imvec(COLOR_TITLE_WIDGET), "Camera Zoom: ");

			auto clamp_Fov = [&] {
				camera.m_angle_fov = std::clamp(camera.m_angle_fov, glm::epsilon<float>() * 2, glm::pi<float>());
			};

			bool updated = false;
			if (ImGui::SliderFloat("Zoom (Angle FOV)", &camera.m_angle_fov, glm::epsilon<float>() * 2, glm::pi<float>(), "%.3f"))
			{
				updated = true;
			}

			if (ImGui::Button(ICON_FA_MAGNIFYING_GLASS_PLUS " Zoom in", ImVec2(100, 25)))
			{
				camera.m_angle_fov += zoom_step;
				updated = true;
			}
			ImGui::SameLine(120);
			if (ImGui::Button(ICON_FA_MAGNIFYING_GLASS_MINUS " Zoom out", ImVec2(100, 25)))
			{
				camera.m_angle_fov -= zoom_step;
				updated = true;
			}

			updated |= RSR.Widget(camera);

			if (updated)
				clamp_Fov();

			return updated;
		}

	} // namespace widget3d

} // namespace bh3d