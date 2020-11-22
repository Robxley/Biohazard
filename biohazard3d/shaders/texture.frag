//Texture shader
//Only use a texture vertex for drawing

#version 330 core
out vec4 FragColor;
  
// the input variable from the vertex shader (same name and same type) 	 
in vec2 vert_texcoord;
uniform sampler2D textureSampler;

void main()
{
	FragColor = texture2D( textureSampler, vert_texcoord );
} 