#version 450 core

layout (location = 0) out vec4 fColor;

in vec4 col;
in vec3 nor;
in vec3 FragPosWorldSpace;
in vec4 FragPosProjectedLightSpace;

uniform vec3 lightDirection;
uniform vec3 lightPos;
uniform vec3 directionalLightColor;

uniform vec3 camPos;

float CalculateDirectionalIllumination(vec3 Nnor, vec3 Nto_light, vec3 NrefLight, vec3 NcamDirection)
{
	float ambient = 0.1f;
	
	float diffuse = max(dot(Nnor,Nto_light),0);

	
	float specular = pow(max(dot(NrefLight,NcamDirection),0),128);

	//float shadow = shadowOnFragment(FragPosProjectedLightSpace);

	return ambient + (diffuse + specular) ;
}

void main()
{
	vec3 Nnor = normalize(nor);
	vec3 Nto_light = - normalize(lightDirection);
	vec3 Nfrom_light =  normalize(lightDirection);
	vec3 NrefLight = reflect(Nfrom_light, Nnor);
	vec3 camDirecrion = camPos - FragPosWorldSpace;
	vec3 NcamDirection = normalize(camDirecrion);

	float phong_directionalL = CalculateDirectionalIllumination(Nnor, Nto_light, NrefLight, NcamDirection);
	vec3 lightContribution= phong_directionalL * directionalLightColor;

	//fColor = vec4(min(lightContribution,1) * col.xyz, 1.f);
	fColor = vec4(lightContribution.xyz* col.xyz,1.0f);
}
