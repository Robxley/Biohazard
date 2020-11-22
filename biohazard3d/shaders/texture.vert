//Texture shader
//Only use a texture vertex for drawing

#version 330 core

layout (location = 0) in vec3 in_Position;		// the position variable has attribute position 0	(following the Biohazard default attribute index)
layout (location = 2) in vec2 in_Coord0;		// the texture variable has attribute position 2	(following the Biohazard default attribute index)

out vec2  vert_texcoord;						// specify a color output to the fragment shader

uniform mat4 proj_view_transform;				//Projection * modelview * transform matrix

void main()
{
    gl_Position = proj_view_transform * vec4(in_Position, 1.0);	// see how we directly give a vec3 to vec4's constructor
	vert_texcoord = in_Coord0;
}
