#version 450 core

in vec2 texCoords;
out vec4 fragColor;

layout(r32ui, binding = 0) uniform uimage2D headPointers;

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

layout(binding = 0, offset=0) uniform atomic_uint nodeCounter;

int maxFragments = 100;

void main()
{
	ivec2 coords = ivec2(gl_FragCoord.xy);
	uint nodeIndex = imageLoad(headPointers, coords).r;

	//if there is no fragments, discard
	if (nodeIndex == 0xFFFFFFFFu)
	{
		//discard;
	}

	vec4 fragments[100];
	float depths[100];
	int count = 0;

	while (nodeIndex != 0xFFFFFFFFu && count < maxFragments)
	{
		fragments[count] = nodes[nodeIndex].color;
		depths[count] = nodes[nodeIndex].depth;
		nodeIndex = nodes[nodeIndex].next;
		count++;
	}

	if(count==0)
	{
		discard;
	}

	//======sort,from back to front
	for (int i = 1; i < count; i++) 
	{
        vec4 tempColor = fragments[i];
        float tempDepth = depths[i];
        int j = i - 1;
        
        // 如果当前片段更远（深度更大），向前移动
        while (j >= 0 && depths[j] < tempDepth) 
		{
            fragments[j + 1] = fragments[j];
            depths[j + 1] = depths[j];
            j--;
        }
        
        fragments[j + 1] = tempColor;
        depths[j + 1] = tempDepth;
    }

	//=======blend
	//color.rgb = src.rgb * src.a +(1 - src.a)*dst.rgb 
    // color.a = src.a +(1 - src.a)*dst.a 
	vec4 accumColor = vec4(0.0);
	for (int i = 0; i < count; i++) 
	{
        vec4 frag = fragments[i];    
        accumColor.rgb = frag.rgb * frag.a + (1.0 - frag.a)* accumColor.rgb;
        accumColor.a = frag.a + (1.0 - frag.a) * accumColor.a;

    }
    
    fragColor = accumColor;
	//fragColor = vec4(accumColor.rgb * accumColor.a, accumColor.a);
}