#pragma once

#include "BH3D_Shader.hpp"
#include <BH3D_GL_GLM_core.hpp>
#include "imgui.h"
#include "imgui_extra_widgets.h"

class ChessBoard2d
{
	enum UNIFORM_LOCATION
	{
		ONE_COLOR = 0,
		TWO_COLOR,
		SIZE_AND_ANGLE
	};


	enum CHESS_BOARD_TYPE
	{
		HORIZ_VERT,
		ANGLE,
		ANGLE_ANTIALIASING
	};

public:
	glm::vec4 m_color_one = { 253.0f / 255.0f, 236.0f / 255.0f, 208.0f / 255.0f, 1.0f };
	glm::vec4 m_color_two = { 119.0f / 255.0f, 149.0f / 255.0f,  86.0f / 255.0f, 1.0f };
	glm::vec3 m_size_and_angle = { 64.0f, 64.0f, 3.0f };

	bh3d::Shader m_shader;

	GLuint m_position_buffer = 0;
	GLuint m_color_buffer = 0;
	GLuint m_vertex_array = 0;
	GLsizei m_vertex_count = 0;
	~ChessBoard2d() { Destroy(); }

	void Destroy()
	{
		BH3D_GL_CHECK_ERROR;

		//Make sure to unbind objects before deleting
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//Delete arrays and buffers
		glDeleteBuffers(1, &m_position_buffer);
		glDeleteBuffers(1, &m_color_buffer);
		glDeleteVertexArrays(1, &m_vertex_array);

		m_position_buffer = 0;
		m_color_buffer = 0;
		m_vertex_array = 0;
		m_vertex_count = 0;
	}

	void Create(float x, float y, float width, float height, int type = CHESS_BOARD_TYPE::ANGLE_ANTIALIASING)
	{

		//Destroy the existing point cloud
		Destroy();

		m_shader.LoadRaw(this->VERTEX_SHADER(), this->FragmentShader(type));

		std::vector<glm::vec3> vPositions = {
			{ -1.0f, -1.0f, 0.0f },	// top right
			{ 1.0f, -1.0f, 0.0f  },	// bottom right
			{ -1.0f, 1.0f, 0.0f  },	// bottom left
			{ 1.0f, 1.0f, 0.0f   }		// top left 
		};

		std::vector<glm::vec4> vColors = {
			{1.0f , 0.0f, 0.0f, 1.0f},
			{0.0f , 1.0f, 0.0f, 1.0f},
			{0.0f , 0.0f, 1.0f, 1.0f},
			{1.0f , 0.0f, 0.0f, 1.0f}
		};

		m_vertex_count = vPositions.size();
		BH3D_GL_CHECK_ERROR;
		//Opengl call
		{
			//VAO setting
			glGenVertexArrays(1, &m_vertex_array);					//Creation VAO	
			assert(m_vertex_array > 0 && "VAO allocation KO");
			glBindVertexArray(m_vertex_array);

			//Opengl Buffer allocation
			m_position_buffer = bh3d::AllocateVBO(vPositions, (GLuint)bh3d::ATTRIB_INDEX::POSITION, GL_STATIC_DRAW);		//Buffer allocation for Position vertex array
			bh3d::UpdateVBO(m_position_buffer, vPositions, GL_STATIC_DRAW);
			assert(m_position_buffer > 0);

			m_color_buffer = bh3d::AllocateVBO(vColors, (GLuint)bh3d::ATTRIB_INDEX::COLOR, GL_STATIC_DRAW);					//Buffer allocation for color vertex array
			bh3d::UpdateVBO(m_color_buffer, vColors, GL_STATIC_DRAW);
			assert(m_color_buffer > 0);
		}

		//Unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	bool IsValid() const {
		return m_position_buffer > 0 && m_color_buffer > 0 && m_vertex_array > 0;
	}

	void Draw(GLenum mode = GL_TRIANGLE_STRIP) const {
		m_shader.Enable();

		m_shader.Send4f(UNIFORM_LOCATION::ONE_COLOR, m_color_one);
		m_shader.Send4f(UNIFORM_LOCATION::TWO_COLOR, m_color_two);
		m_shader.Send3f(UNIFORM_LOCATION::SIZE_AND_ANGLE, m_size_and_angle);

		BH3D_GL_CHECK_ERROR;
		assert(IsValid() && "Allocate the buffer before updating - Call CreateCameraTrajectoryVBO");
		glBindVertexArray(m_vertex_array);
		glDrawArrays(mode, 0, m_vertex_count);
		m_shader.Disable();
	}

	void Widget()
	{
		ImGui::ColorPicker3("One", &m_color_one[0]);
		ImGui::ColorPicker3("Two", &m_color_two[0]);
		ImGui::DragFloat2("Size", &m_size_and_angle[0]);
		ImGui::DragFloat("Rad", &m_size_and_angle[2]);
	}


	static inline constexpr const char* FragmentShader(int type = CHESS_BOARD_TYPE::ANGLE_ANTIALIASING)
	{
		switch (type)
		{
		case 0:
			return FRAGMENT_SHADER_SAMPLE();
		case 1:
			return VERTEX_SHADER();
		default:
			return FRAGMENT_SHADER_ANTIALIASING();
			break;
		}
	}

	static inline constexpr const char* VERTEX_SHADER() {
		return
			R"(
				#version 330 core

				layout(location = 0) in vec3 in_Position;	// the position variable has attribute position 0					
				layout(location = 1) in vec4 in_Color;		// the color variable has attribute position 1						

				out vec4  vert_color;						// specify a color output to the fragment shader					

				void main()
				{
					gl_Position =  vec4(in_Position, 1.0);	// vertex projection on the screen	
					vert_color = in_Color;					// forward color vertex				
				}																													
			)";
	}
	static inline constexpr const char* FRAGMENT_SHADER_SAMPLE()
	{
		return
			R"(
			#version 460 core

			out vec4 FragColor;

			layout(location = 0) uniform vec4 color_one;
			layout(location = 1) uniform vec4 color_two;
			layout(location = 2) uniform vec3 size_and_angle;

			void main()
			{
				// Calculate normalized coordinates
				vec2 normalizedCoords = gl_FragCoord.xy / size_and_angle.xy;

				// Calculate checkerboard pattern
				bool isDark = mod(floor(normalizedCoords.x) + floor(normalizedCoords.y), 2.0) == 0.0;
				FragColor = isDark ? color_one  :  color_two;
			}
		)";
	}

	static inline constexpr const char* FRAGMENT_SHADER()
	{
		return 
		R"(
			#version 460 core

			out vec4 FragColor;

			layout(location = 0) uniform vec4 color_one;
			layout(location = 1) uniform vec4 color_two;
			layout(location = 2) uniform vec3 size_and_angle;

			void main()
			{
				// Calculate normalized coordinates
				vec2 normalizedCoords = gl_FragCoord.xy / size_and_angle.xy;

				// Apply rotation if needed
				float angle = radians(size_and_angle.z);
				mat2 rotationMatrix = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
				normalizedCoords = rotationMatrix * (normalizedCoords - 0.5) + 0.5;

				// Calculate checkerboard pattern
				bool isDark = mod(floor(normalizedCoords.x) + floor(normalizedCoords.y), 2.0) == 0.0;
				FragColor = isDark ? color_one  :  color_two;
			}
		)";
	}

	static inline constexpr const char* FRAGMENT_SHADER_ANTIALIASING()
	{
		return
			R"(
				#version 460 core

				out vec4 FragColor;

				layout(location = 0) uniform vec4 color_one;
				layout(location = 1) uniform vec4 color_two;
				layout(location = 2) uniform vec3 size_and_angle;

				float sf = 0.01; //smooth factor

				void main()
				{
					// Calculate normalized coordinates
					vec2 normalizedCoords = gl_FragCoord.xy / size_and_angle.xy;

					// Apply rotation if needed
					float angle = radians(size_and_angle.z);
					mat2 rotationMatrix = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
					normalizedCoords = rotationMatrix * (normalizedCoords - 0.5) + 0.5;

					// Calculate checkerboard pattern
					bool isDark = mod(floor(normalizedCoords.x) + floor(normalizedCoords.y), 2.0) == 0.0;

					// Calculate the gradient factor based on the distance to the nearest edge of the square
					float gradientFactorX = smoothstep(0.5-sf, 0.5+sf, abs(fract(normalizedCoords.x) - 0.5));
					float gradientFactorY = smoothstep(0.5-sf, 0.5+sf, abs(fract(normalizedCoords.y) - 0.5));
					float gradientFactor = max(gradientFactorX, gradientFactorY);

					// Interpolate between the two colors with the gradient factor
					vec4 blendedColor = mix(color_one, color_two, gradientFactor);

					// Set the fragment color based on the pattern
					FragColor = isDark ? blendedColor : mix(color_two, color_one, gradientFactor);
				}

		)";
	}
};