#version 330 core
out vec4 FragColor;
  
// the input variable from the vertex shader (same name and same type) 
in vec4 vert_color; 		 
in vec2 vert_texcoord;

uniform sampler2D textureSampler;

void main()
{
	vec4 tex_color = texture2D( textureSampler, vert_texcoord );
	FragColor = vert_color * tex_color.r;
} 