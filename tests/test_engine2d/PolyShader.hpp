#pragma once

#include "BH3D_Shader.hpp"
#include "BH3D_VBO.hpp"
#include <BH3D_GL_GLM_core.hpp>
#include <BH3D_TinyShader.hpp>

namespace PolyShader
{

	namespace Circle2d
	{

		struct Vertex
		{
			glm::vec3 m_pos_and_rad;	// position (x,y) and radius (z)
			glm::vec4 m_color;			// color
			glm::vec2 m_coord0;			//texutre coord


			static GLuint  AttribCount() { return 3; }

			static std::vector<GLuint> AttribIndex() { 
				return { 
					(GLuint)bh3d::ATTRIB_INDEX::POSITION,
					(GLuint)bh3d::ATTRIB_INDEX::COLOR,
					(GLuint)bh3d::ATTRIB_INDEX::COORD0
			}; }

			static std::vector<GLenum> AttribType() {
				return { GL_FLOAT,GL_FLOAT,GL_FLOAT };
			}

			static std::vector <GLuint> AttribSize() {
				return {
					decltype(Vertex().m_pos_and_rad)::length(),
					decltype(Vertex().m_color)::length(),
					decltype(Vertex().m_coord0)::length()
				};
			}

			static std::vector<GLuint> AttribOffset() {
				return {
					sizeof(decltype(Vertex().m_pos_and_rad)),
					sizeof(decltype(Vertex().m_color)),
					sizeof(decltype(Vertex().m_coord0))
				};
			}
		};

		static auto CreateVBO(const std::vector<Vertex>& circles)
		{
			bh3d::VBO vbo;
			vbo.AddStructArrayBufferData(
				Vertex::AttribCount(),
				Vertex::AttribIndex(),
				Vertex::AttribType(),
				Vertex::AttribSize(),
				Vertex::AttribOffset(),
				0,
				circles.size(),
				circles.data()
			);
			return vbo;
		}

		inline constexpr const char* VERTEX_SHADER() {
			return
				R"(																														
				#version 330 core																										
																																		
				layout(location = 0) in vec3 in_Position;		// the position variable has attribute position 0						
				layout(location = 1) in vec4 in_Color;			// the color variable has attribute position 1							
				layout(location = 2) in vec2 in_Coord0;			// the texture variable has attribute position 2						
																																		
				out vec4  vert_color;							// specify a color output to the fragment shader						
				out vec2  vert_texcoord;						// specify a color output to the fragment shader						
																																		
				uniform mat4 proj_view_transform;				//Projection * modelview * transform matrix								
																																		
				void main()																												
				{																														
					gl_Position = proj_view_transform * vec4(in_Position, 1.0);					// vertex projection on the screen		
					vert_color = in_Color;														// forward color vertex					
					vert_texcoord = in_Coord0;													// forward texture vertex				
				}																														
			)";
		}


		inline constexpr const char* FRAGMENT_SHADER() {
			return
				R"(																											
				#version 330 core 																						
				out vec4 FragColor;																						
																														
				in vec4 vert_color; // the input variable from the vertex shader (same name and same type)  			
				in vec2 vert_texcoord;																					
																														
				uniform sampler2D textureSampler;																		
																														
				void main()																								
				{																										
					vec4 tex_color = texture2D(textureSampler, vert_texcoord);											
					FragColor = (vert_color * tex_color);																
				}																										
			)";
		}


	}

	

}