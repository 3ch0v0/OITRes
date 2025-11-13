#version 450 core

layout(location = 0) in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D transparentTex;

void main()
{
	vec4 transparentColor = texture(transparentTex, texCoords);
    
    if (transparentColor.a < 0.001) {
        discard;
    }
    fragColor = transparentColor;

}