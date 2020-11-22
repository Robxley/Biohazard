//Color shader
//Only use a color vertex for drawing

#version 330 core
out vec4 FragColor;
  
// the input variable from the vertex shader (same name and same type) 
in vec4 vert_color; 		 
void main()
{
	FragColor = vert_color;
} 