#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec4 vCol;
layout (location = 2) in vec3 vNor;


out vec4 FragColor;
out vec3 nor;
out vec3 FragPosWorldSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vPos;
    FragColor = vCol;
    nor = mat3(transpose(inverse(model))) * vNor;
	FragPosWorldSpace = vec3(model * vPos);
}