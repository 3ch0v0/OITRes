#version 450 core

in vec2 TexCoord;
out vec4 FragColor;

#define MAX_FRAGMENTS 32

struct FragmentNode {
    vec4 color;
    float depth;
    uint next;
};

layout(binding = 0, r32ui) uniform uimage2D headPointers;

layout(std430, binding = 1) buffer FragmentList {
    FragmentNode fragments[];
};

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);
    uint index = imageLoad(headPointers, coords).r;
    
    // 如果没有透明片段，丢弃（保留不透明物体）
    if (index == 0xFFFFFFFFu) {
        discard;
    }
    
    // 收集所有片段
    FragmentNode sortedFragments[MAX_FRAGMENTS];
    int count = 0;
    
    while (index != 0xFFFFFFFFu && count < MAX_FRAGMENTS) {
        sortedFragments[count] = fragments[index];
        index = fragments[index].next;
        count++;
    }
    
    // 插入排序（按深度从近到远）
    for (int i = 1; i < count; i++) {
        FragmentNode key = sortedFragments[i];
        int j = i - 1;
        while (j >= 0 && sortedFragments[j].depth > key.depth) {
            sortedFragments[j + 1] = sortedFragments[j];
            j--;
        }
        sortedFragments[j + 1] = key;
    }
    
    // 从后向前混合（预乘Alpha混合）
    vec4 color = vec4(0.0);
    for (int i = count - 1; i >= 0; i--) {
        vec4 src = sortedFragments[i].color;
        // 标准 Alpha 混合
        color.rgb = src.rgb * src.a + color.rgb * (1.0 - src.a);
        color.a = src.a + color.a * (1.0 - src.a);
    }
    
    FragColor = color;
}