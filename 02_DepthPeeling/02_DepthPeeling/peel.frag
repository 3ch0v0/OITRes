#version 450 core

in vec4 FragColor;
layout (location = 0) out vec4 outColor;

uniform sampler2D depthTexture;
float depthBias = 0.0001;

void main()
{
    
    vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);
    float prevDepth = texture(depthTexture, uv).r;
    

    // keep farther frags
    if (gl_FragCoord.z <= prevDepth + depthBias) {
        discard;
    }
    
    outColor = FragColor;
}