#version 450 core

#extension GL_NV_gpu_shader5 : enable
#extension GL_NV_fragment_shader_interlock : enable

layout(early_fragment_tests) in;
layout(pixel_interlock_unordered) in;

layout(binding =0, rgba32ui) coherent uniform uimage2D fragmentCounterTex;   // R:fragment count G:1st closest depth B:2nd closest depth
layout(binding=1, rgba16f) coherent uniform image2D accTex;                 // RGB: accumulated pre-multiplied color +col*alpha; A: accumulated *(1 - alpha)
layout(binding=2, rgba16f) coherent uniform image2D averageTex;             // RGB: +col; A:+alpha
layout(binding=3, rgba16f) coherent uniform image2DArray bucketTex;         // RGB: OIT col; A:OIT alpha

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
    vec3 finalColor = col.rgb * texColor.rgb * lightContribution;
    vec4 fragColor = vec4(finalColor, clamp(col.a * texColor.a,0.0001,0.9999));
    vec4 premulColor = vec4(fragColor.rgb * fragColor.a, fragColor.a);
    
    ivec2 coords = ivec2(gl_FragCoord.xy);
    float depth = gl_FragCoord.z;

    beginInvocationInterlockNV();
    //Accmulation
    vec4 accumColor = imageLoad(accTex, coords);
    accumColor.rgb += premulColor.rgb;
    accumColor.a *= (1-fragColor.a);
    imageStore(accTex, coords, accumColor);

    //Average Color
    vec4 averageColor = imageLoad(averageTex, coords);
    averageColor.rgb += fragColor.rgb;
    averageColor.a += fragColor.a;
    imageStore(averageTex, coords, averageColor);

    //bucket
    uvec4 counterData = imageLoad(fragmentCounterTex,coords);
    uint fragNum = counterData.r +1;
    float depth1 = uintBitsToFloat(counterData.g);
    float depth2 = uintBitsToFloat(counterData.b);

    if(depth<depth1)
    {       
        vec4 preFrag=imageLoad(bucketTex, ivec3(coords,0));
        imageStore(bucketTex, ivec3(coords,1), preFrag);
        depth2 = depth1;

        imageStore(bucketTex, ivec3(coords,0), premulColor);
        depth1 = depth;
    }

    else if(depth<depth2)
    {
        imageStore(bucketTex, ivec3(coords,1), premulColor);
        depth2 = depth;
    }
    imageStore(fragmentCounterTex, coords, uvec4(fragNum, floatBitsToUint(depth1), floatBitsToUint(depth2),0));
    endInvocationInterlockNV();
}