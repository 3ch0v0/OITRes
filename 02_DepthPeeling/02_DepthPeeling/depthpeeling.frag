#version 450 core

in vec4 FragColor;
layout (location = 0) out vec4 outColor;

uniform sampler2D depthTexture;

void main()
{
    // 读取前一层的深度值
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);
    float prevDepth = texture(depthTexture, uv).r;
    
    // 关键：剔除深度 <= 前一层深度的片段
    // 只保留更深（更远）的片段
    if (gl_FragCoord.z <= prevDepth) {
        discard;
    }
    
    // 输出颜色
    outColor = FragColor;
}