#version 430 core

in vec4 vertexColor;

// A-Buffer 数据结构
struct FragmentNode {
    vec4 color;
    float depth;
    uint next;
};

// 头指针纹理（每个像素存储链表头索引）
layout(binding = 0, r32ui) uniform uimage2D headPointers;

// 原子计数器（分配新节点）
layout(binding = 0, offset = 0) uniform atomic_uint fragmentCounter;

// 片段链表存储
layout(std430, binding = 1) buffer FragmentList {
    FragmentNode fragments[];
};

void main() {
    // 分配新的片段节点
    uint index = atomicCounterIncrement(fragmentCounter);
    
    // 原子交换：将新节点插入链表头部
    uint oldHead = imageAtomicExchange(
        headPointers, 
        ivec2(gl_FragCoord.xy), 
        index
    );
    
    // 存储片段信息
    fragments[index].color = vertexColor;
    fragments[index].depth = gl_FragCoord.z;
    fragments[index].next = oldHead;
    
    // 不输出颜色（只构建链表）
    discard;
}