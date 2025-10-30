#version 450 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D colorTexture;
uniform vec3 backgroundColor;

void main()
{
    vec4 color = texture(colorTexture, TexCoords);
    
    // 将累积的透明层合成到背景色上
    // 公式: result = color.rgb + background * (1.0 - color.a)
    //vec3 result = color.rgb + backgroundColor * (1.0 - color.a);
    vec3 result = color.rgb + backgroundColor * color.a;
    
    FragColor = vec4(result, 1.0);
}