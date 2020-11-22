//Color shader
//Only use a color vertex for drawing

#version 330 core

layout (location = 0) in vec3 in_Position;		// the position variable has attribute position 0	(following the Biohazard default attribute index)
layout (location = 1) in vec4 in_Color;			// the color variable has attribute position 1		(following the Biohazard default attribute index)

out vec4  vert_color;	
uniform mat4 proj_view_transform;				//Projection * modelview * transform matrix

void main()
{
    gl_Position = proj_view_transform * vec4(in_Position, 1.0);	// see how we directly give a vec3 to vec4's constructor
    vert_color = in_Color;														// set the output variable to a dark-red color
}
