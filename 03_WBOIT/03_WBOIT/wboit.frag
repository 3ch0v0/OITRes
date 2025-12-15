#version 450 core

layout (location = 0) out vec4 accumColor;
layout (location = 1) out float accumAlpha;

in vec4 col;
in vec3 nor;
in vec2 texCoords;
in vec3 FragPosWorldSpace;
in float fragDepth;

uniform sampler2D mainTex;
uniform vec3 lightDirection;
uniform vec3 lightPos;
uniform vec3 directionalLightColor;

uniform vec3 camPos;

float CalculateDirectionalIllumination(vec3 Nnor, vec3 Nto_light, vec3 NrefLight, vec3 NcamDirection)
{
	float ambient = 0.5f;
	
	float diffuse = max(dot(Nnor,Nto_light),0);
	
	float specular = pow(max(dot(NrefLight,NcamDirection),0),128);

	return ambient + (diffuse + specular) ;
}

void main()
{
    vec4 texColor = texture(mainTex, texCoords);
    vec3 Nnor = normalize(nor);
    if (!gl_FrontFacing) 
    {
        Nnor = -Nnor;
    }

    vec3 Nto_light = - normalize(lightDirection);
    vec3 Nfrom_light =  normalize(lightDirection);
    vec3 NrefLight = reflect(Nfrom_light, Nnor);
    vec3 camDirecrion = camPos - FragPosWorldSpace;
    vec3 NcamDirection = normalize(camDirecrion);

    float phong_directionalL = CalculateDirectionalIllumination(Nnor, Nto_light, NrefLight, NcamDirection);
    vec3 lightContribution= phong_directionalL * directionalLightColor;
    //vec3 lightContribution =  directionalLightColor;

    //w=alpha*clmap(power/depth,min,max)
    //float weight = col.a * clamp(1.0 - fragDepth, 0.0, 1.0);
    //float weight = col.a * max(0.01, min(3000.0, 10.0 / (pow(fragDepth, 3.0) + 0.00001)));

    //col.rgb *= lightContribution;
    float weight = max(min(1.0, max(max(col.r, col.g), col.b) * col.a), col.a) * clamp(0.03 / (1e-5 + pow(fragDepth / 200, 4.0)), 1e-2, 3e3);
    weight = min(weight, 1000.0);
    accumColor =vec4(col.rgb * col.a * lightContribution * texColor.rgb, col.a)* weight;
    
    accumAlpha = col.a;
}