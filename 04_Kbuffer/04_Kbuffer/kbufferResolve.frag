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

layout(binding = 2, offset=0) uniform atomic_uint nodeCounter;

//int maxFragments = 100;

#define K_SIZE 8

void main()
{
	ivec2 coords = ivec2(gl_FragCoord.xy);
	uint nodeIndex = imageLoad(headPointers, coords).r;

	//if there is no fragments, discard
	if (nodeIndex == 0xFFFFFFFFu)
	{
		discard;
	}

	vec4 fragments[K_SIZE];
	float depths[K_SIZE];
	int count = 0;

	while (nodeIndex != 0xFFFFFFFFu)
	{
		vec4 col = nodes[nodeIndex].color;
		float depth = nodes[nodeIndex].depth;
		
		if (count < K_SIZE)
		{
			fragments[count] = col;
			depths[count] = depth;
			count++;
		}
		else
		{
			int maxIdx = 0;
			float maxD = depths[0];
			
			for(int i = 1; i < K_SIZE; i++) 
			{
				if(depths[i] > maxD) 
				{
					maxD = depths[i];
					maxIdx = i;
				}
			}

			if (depth < maxD)
			{
				fragments[maxIdx] = col;
				depths[maxIdx] = depth;
			}
            
		} 

		
		nodeIndex = nodes[nodeIndex].next; 
	}
        

	if(count==0)
	{
		discard;
	}

	//======sort,from back to front=======
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

	//=======blend==================
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

}