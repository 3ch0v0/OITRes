#version 450 core

layout(location = 0) in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D colorTex;
uniform sampler2D alphaTex;

void main()
{
	vec4 accumColor = texture(colorTex, texCoords);
	float accumAlpha = texture(alphaTex, texCoords).r;

	//if (accumAlpha >=1.0)
	//{discard;}
	//vec3 finalColor = accumColor.rgb / max(accumAlpha, 0.0001);

	float finalAlpha = 1.0 - accumAlpha;
	
	//fragColor = vec4(finalColor, finalAlpha);
	fragColor = vec4(accumColor.rgb / max(accumColor.a, 1e-5), accumAlpha);

}