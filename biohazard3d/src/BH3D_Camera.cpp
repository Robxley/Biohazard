#include <algorithm>
#include <iostream>

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "BH3D_Camera.hpp"
#include "BH3D_Event.hpp"

namespace bh3d
{

	void Camera::LookAt(const glm::vec3 & position, const glm::vec3 & target, const glm::vec3 & up)
	{
		glm::mat4 tmp_modelview = glm::lookAt(position, target, up);

		auto isfinite = [](const glm::mat4 & mat) -> bool {
			auto * first = glm::value_ptr(mat);
			auto * last = first + 16;
			for (; first != last; ++first) {
				if (!std::isfinite(*first)) {
					return false;
				}
			}
			return true;
		};
		if (isfinite(tmp_modelview))
			m_modelview = tmp_modelview;
	}

	Camera::LookAtInfos Camera::ExtractLookAtInfos(const glm::mat4 & lookat, const glm::vec4 & direction, const glm::vec4 & up)
	{
		glm::vec3 position = glm::column(lookat, 3);

		auto new_vec = [&](const glm::vec4& v) ->glm::vec3 { return glm::normalize(glm::vec3(lookat * v)); };

		glm::vec3 new_dir = new_vec(direction);
		glm::vec3 new_up = new_vec(up);

		return std::tuple(position, new_dir, new_up);
	}

	void CameraEngine::LookAround(const Mouse &m_mouse)
	{

		if (m_mouse == Mouse::Event::UP || m_mouse == Mouse::Event::NONE)
			return;

		if (m_mouse == Mouse::Event::WHEEL)
		{
			m_zoom += m_zoom_speed * m_mouse.GetData();
			m_zoom = std::clamp(m_zoom, m_zoom_min, m_zoom_max);
		}
		else if (m_mouse == Mouse::Event::MOVE || m_mouse & Mouse::Button::LEFT)
		{
			if (m_mouse & Mouse::Button::LEFT)
			{
				int delta_x = m_mouse.GetRelativePosX();
				int delta_y = m_mouse.GetRelativePosY();

				if (float angle_x = delta_x * m_mouse_speed; std::isfinite(angle_x))
				{
					if (float angle_y = delta_y * m_mouse_speed; std::isfinite(angle_y))
					{
						glm::mat4 newRotate = glm::rotate(glm::mat4(1.0f), angle_x, glm::vec3(0, 1, 0));
						newRotate = glm::rotate(newRotate, angle_y, glm::vec3(1, 0, 0));
						m_transform = newRotate * m_transform;
					}
				}
			}

		}
		else if (m_mouse & Mouse::Button::RIGHT)
		{
			m_zoom = m_default_zoom;
			m_transform = glm::mat4( 1.0f );
		}

		LookAt();
	}

	void CameraEngine::FreeFlight(const Mouse &m_mouse)
	{

		if (m_mouse == Mouse::Event::UP || m_mouse == Mouse::Event::NONE)
			return;

		if (m_mouse == Mouse::Event::WHEEL)
		{
			m_movement_speed += m_mouse.GetData();
		}
		else if (m_mouse == Mouse::Event::MOVE && (m_mouse & Mouse::Button::LEFT || m_mouse & Mouse::Button::RIGHT))
		{
			int delta_x = m_mouse.GetRelativePosX();
			int delta_y = m_mouse.GetRelativePosY();

			glm::vec3 right_direction = glm::normalize(glm::cross(m_up, m_direction));
			if (float angle_x = delta_x * m_mouse_speed; std::isfinite(angle_x))
			{
				glm::vec3 up_directiction = glm::normalize(glm::cross(right_direction, m_direction));
				m_direction = glm::rotate(m_direction, angle_x, up_directiction);
			}

			if (float angle_y = delta_y * m_mouse_speed; std::isfinite(angle_y))
			{
				glm::vec3 right_direction = glm::normalize(glm::cross(m_up, m_direction));
				m_direction = glm::rotate(m_direction, angle_y, right_direction);
			}
		}


		if (m_mouse & Mouse::Button::RIGHT && m_mouse & Mouse::Button::LEFT)
		{
			MoveBackWard();
		}	
		else if (m_mouse & Mouse::Button::LEFT)
		{
			MoveForward();
		}
	
		LookAt();
	}

	void CameraTrajectory::TrajectoryFlight(const Mouse &m_mouse)
	{

		if (m_mouse == Mouse::Event::UP || m_mouse == Mouse::Event::NONE)
			return;

		if (m_mouse == Mouse::Event::WHEEL)
		{
			int wheel_value = m_mouse.GetData();
			if (wheel_value > 0) {
					m_key++;
			}
			else if (wheel_value < 0){
				m_key--;
			}
			ExtractLookAtParams();
		}
		else if (m_mouse == Mouse::Event::MOVE && m_mouse & Mouse::Button::LEFT)
		{
			int delta_x = m_mouse.GetRelativePosX();
			int delta_y = m_mouse.GetRelativePosY();

			glm::vec3 right_direction = glm::normalize(glm::cross(m_up, m_direction));
			if (float angle_x = delta_x * m_mouse_speed; std::isfinite(angle_x))
			{
				glm::vec3 up_directiction = glm::normalize(glm::cross(right_direction, m_direction));
				m_direction = glm::rotate(m_direction, angle_x, up_directiction);
			}

			if (float angle_y = delta_y * m_mouse_speed; std::isfinite(angle_y))
			{
				glm::vec3 right_direction = glm::normalize(glm::cross(m_up, m_direction));
				m_direction = glm::rotate(m_direction, angle_y, right_direction);
			}
		}

		if ( (m_mouse & Mouse::Button::LEFT) && (m_mouse & Mouse::Button::RIGHT) )
		{
			m_position += (m_direction)*(m_movement_speed * m_fps_elapse_time);
		}
		else if (m_mouse & Mouse::Button::RIGHT)
		{
			m_position -= (m_direction)*(m_movement_speed * m_fps_elapse_time);
		}

		LookAt();
	}

}