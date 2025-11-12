#version 450 core

layout (location = 0) out vec4 accumColor;
layout (location = 1) out float accumAlpha;

in vec4 col;
in float fragDepth;

void main()
{
    //w=alpha*clmap(power/depth,min,max)
    //float weight = col.a * clamp(1.0 - fragDepth, 0.0, 1.0);
    //float weight = col.a * max(0.01, min(3000.0, 10.0 / (pow(fragDepth, 3.0) + 0.00001)));
    float weight = max(min(1.0, max(max(col.r, col.g), col.b) * col.a), col.a) * clamp(0.03 / (1e-5 + pow(fragDepth / 200, 4.0)), 1e-2, 3e3);
    accumColor =vec4(col.rgb * col.a, col.a)* weight;
    accumAlpha = col.a;
}