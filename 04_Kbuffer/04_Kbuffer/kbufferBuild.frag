#version 450 core

layout(early_fragment_tests) in;//在Shader中对显存的直接写入操作时，驱动程序会强制关闭 Early-Z，改为使用 Late-Z，导致明明没有通过深度测试的片元依旧写入列表

layout(r32ui, binding = 0) uniform uimage2D headPointers;

in vec4 col;
in vec3 nor;
in vec2 texCoords;
in vec3 FragPosWorldSpace;

uniform sampler2D mainTex;
uniform vec3 lightDirection;
uniform vec3 lightPos;
uniform vec3 directionalLightColor;
uniform vec3 camPos;

struct ListNode
{
	vec4 color;
	float depth;
	uint next;
	uint padding[2];

};

layout(std430, binding = 1) buffer FragmentList
{
	ListNode nodes[];
};

layout(binding = 2, offset=0) uniform atomic_uint nodeCounter;

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

	uint nodeIndex = atomicCounterIncrement(nodeCounter);

	if (nodeIndex >= 16777216u)
	{
		discard;
	}

	ivec2 coords = ivec2(gl_FragCoord.xy);
	uint prevHead = imageAtomicExchange(headPointers, coords, nodeIndex);
	nodes[nodeIndex].color = vec4(col.rgb * lightContribution * texColor.rgb,col.w);
	nodes[nodeIndex].depth = gl_FragCoord.z;
	nodes[nodeIndex].next = prevHead;
}