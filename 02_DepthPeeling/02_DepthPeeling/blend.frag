#version 450 core

in vec2 TexCoords;
layout (location = 0) out vec4 FragColor;

uniform sampler2D colorTexture;

void main()
{
    // 简单传递颜色，混合由 OpenGL 状态机完成
    // glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA)
    FragColor = texture(colorTexture, TexCoords);
}