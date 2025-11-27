#version 450 core

layout (location = 0) out vec4 outColor;

in vec4 FragColor;
in vec3 nor;
in vec3 FragPosWorldSpace;

uniform vec3 lightDirection;
uniform vec3 lightPos;
uniform vec3 directionalLightColor;
uniform vec3 camPos;

uniform sampler2D depthTexture;
float depthBias = 0.000001;

float CalculateDirectionalIllumination(vec3 Nnor, vec3 Nto_light, vec3 NrefLight, vec3 NcamDirection)
{
	float ambient = 0.15f;
	
	float diffuse = max(dot(Nnor,Nto_light),0);
	
	float specular = pow(max(dot(NrefLight,NcamDirection),0),128);

	return ambient + (diffuse + specular) ;
}


void main()
{
    
    vec2 uv = gl_FragCoord.xy / vec2(800, 600);
    float prevDepth = texture(depthTexture, uv).r;

    // keep farther frags
    if (gl_FragCoord.z <= prevDepth+depthBias) 
    {
        discard;
    }

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
    //lightContribution=min(1.0,lightContribution );
    outColor = vec4(FragColor.rgb * lightContribution, FragColor.a);
}