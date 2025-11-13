#version 450 core

layout(r32ui, binding = 0) uniform uimage2D headPointers;

in vec4 col;

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

void main()
{
	uint nodeIndex = atomicCounterIncrement(nodeCounter);

	if (nodeIndex >= 16777216u)
	{
		discard;
	}

	ivec2 coords = ivec2(gl_FragCoord.xy);
	uint prevHead = imageAtomicExchange(headPointers, coords, nodeIndex);
	nodes[nodeIndex].color = col;
	nodes[nodeIndex].depth = gl_FragCoord.z;
	nodes[nodeIndex].next = prevHead;
}