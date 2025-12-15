#version 450 core

out vec4 fColor;

in vec2 fTexCoord;

uniform sampler2D screenTexture;

void main()
{
	fColor= texture(screenTexture, fTexCoord);
}