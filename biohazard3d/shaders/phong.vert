//Phong (lighting) shader
//Only use a color vertex for drawing

//Based on https://www.geeks3d.com/20091013/shader-library-phong-shader-with-multiple-lights-glsl/

#version 330 core

layout (location = 0) in vec3 in_Position;		// the position variable has attribute position 0			(following the Biohazard default attribute index)
layout (location = 3) in vec3 in_Normal;		// the normal variable has attribute position 3				(following the Biohazard default attribute index)


out vec3 FragPos;
out vec3 Normal;

uniform mat4 transform;
uniform mat4 modelview;
uniform mat4 projection;

void main()
{
    FragPos = vec3(transform * vec4(in_Position, 1.0));
    Normal = mat3(transpose(inverse(transform))) * in_Normal;  
    gl_Position = projection * modelview * vec4(FragPos, 1.0);
}
