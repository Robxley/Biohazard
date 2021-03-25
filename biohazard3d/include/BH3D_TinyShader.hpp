#pragma once

/// <summary>
/// Basic shader used by default in TinyEngine  (Engine example using Biohasard3D)
/// </summary>
namespace bh3d
{
	namespace TinyShader
	{
		inline constexpr const char * DEFAULT_VERTEX() {
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


		inline constexpr const char * DEFAULT_FRAGMENT() {
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

		inline constexpr const char * DEFAULT_GREY_FRAGMENT() {
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
					FragColor = (vert_color * tex_color.r);
				}			
			)";
		}

		inline constexpr const char * COLOR_VERTEX() {
			return
			R"(
				#version 330 core

				layout(location = 0) in vec3 in_Position;	// the position variable has attribute position 0					
				layout(location = 1) in vec4 in_Color;		// the color variable has attribute position 1						

				out vec4  vert_color;						// specify a color output to the fragment shader					

				uniform mat4 proj_view_transform;			//Projection * modelview * transform matrix							
	
				void main()
				{
					gl_Position = proj_view_transform * vec4(in_Position, 1.0);	// vertex projection on the screen	
					vert_color = in_Color;										// forward color vertex				
				}																													
			)";
		}

		inline constexpr const char * COLOR_FRAGMENT() {
			return
			R"(
				#version 330 core
				out vec4 FragColor;
				in vec4 vert_color; // the input variable from the vertex shader (same name and same type)
				void main()
				{
					FragColor = vert_color;
				}
			)";
		}

		inline constexpr const char * TEXTURE_VERTEX() {
			return
			R"(																														
				#version 330 core

				layout(location = 0) in vec3 in_Position;		// the position variable has attribute position 0
				layout(location = 2) in vec2 in_Coord0;			// the texture variable has attribute position 2

				out vec2  vert_texcoord;						// specify a color output to the fragment shader

				uniform mat4 proj_view_transform;				//Projection * modelview * transform matrix

				void main()
				{
					gl_Position = proj_view_transform * vec4(in_Position, 1.0); // vertex projection on the screen
					vert_texcoord = in_Coord0;									 // forward texture vertex
				}
			)";
		}

		inline constexpr const char * TEXTURE_FRAGMENT() {
			return
			R"(
				#version 330 core
				out vec4 FragColor;
				in vec2 vert_texcoord;
				uniform sampler2D textureSampler;
				void main()
				{
					FragColor = texture2D(textureSampler, vert_texcoord);
				}
			)";
		}

		inline constexpr const char * TEXTURE_GREY_FRAGMENT() {
			return
			R"(
				#version 330 core 
				out vec4 FragColor;
				in vec2 vert_texcoord;
				uniform sampler2D textureSampler;
				void main()
				{
					vec4 color = texture2D( textureSampler, vert_texcoord );
					FragColor = vec4(color.r, color.r, color.r, color.r);
				}
			)";
		}

		inline constexpr const char * FONT_VERTEX() {
			return
			R"(
				#version 330 core
				
				layout(location = 0) in vec2 in_Vertex;
				layout(location = 2) in vec2 in_Coord0;

				out vec2 texCoord;
				
				uniform mat4 projection;
				uniform ivec2 offset;
				void main()
				{
					texCoord = in_Coord0;
					gl_Position = projection * vec4(in_Vertex + offset, 1.0, 1.0);
				}
			)";
		}

		inline constexpr const char * FONT_FRAGMENT() {
			return
			R"(
				#version 330 core

				in vec2 texCoord;
				out vec4 out_Color;

				uniform sampler2D textureSampler;
				uniform vec4 uColor;
				
				void main()
				{
					vec4 color_t = texture2D(textureSampler, texCoord);
					out_Color = color_t * uColor;
				}				
			)";
		}

		inline constexpr const char * PHONG_VERTEX() {
			return
			R"(
				#version 330 core
				
				layout (location = 0) in vec3 in_Position;
				layout (location = 1) in vec4 in_Color;								
				layout (location = 3) in vec3 in_Normal;								

				out vec3 FragPos;
				out vec3 Normal;
				out vec4 Color;

				uniform mat4 transform;
				uniform mat4 modelview;
				uniform mat4 projection;

				void main()
				{
					FragPos = vec3(transform * vec4(in_Position, 1.0));
					Normal = mat3(transpose(inverse(transform))) * in_Normal;
					Color =  in_Color;
					gl_Position = projection * modelview * vec4(FragPos, 1.0);
				}
			)";
		}

		inline constexpr const char * PHONG_FRAGMENT() {
			return 
			R"(
				#version 330 core

				out vec4 FragColor;
				in vec3 Normal;
				in vec3 FragPos;
				in vec4 Color;

				uniform vec3 lightPos;
				uniform vec3 viewPos;
				uniform vec3 lightColor;
				uniform vec3 objectColor;

				void main()
				{
					// ambient
					float ambientStrength = 0.1;
					vec3 ambient = ambientStrength * lightColor;

					// diffuse
					vec3 norm = normalize(Normal);
					vec3 lightDir = normalize(lightPos - FragPos);
					float diff = max(dot(norm, lightDir), 0.0);
					vec3 diffuse = diff * lightColor;

					// specular
					float specularStrength = 0.5;
					vec3 viewDir = normalize(viewPos - FragPos);
					vec3 reflectDir = reflect(-lightDir, norm);
					float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
					vec3 specular = specularStrength * spec * lightColor;
					vec3 mixColor = Color.rgb + objectColor;
					vec3 result = (ambient + diffuse + specular) * mixColor;
					FragColor = vec4(result, Color.a);
				}
			)";
		}

	}

}  // Namespace bh3d


