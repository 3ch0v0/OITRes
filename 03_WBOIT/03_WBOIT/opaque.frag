#version 450 core

layout (location = 0) out vec4 fColor;

in vec4 col;


void main()
{
	fColor = col;
}
