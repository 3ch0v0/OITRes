#version 450 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D colorTexture;
uniform vec4 backgroundColor;

void main()
{
    vec4 color = texture(colorTexture, TexCoords);
    

    //vec3 result = color.rgb*color.a + backgroundColor.rgb * (1.0 - color.a);
    vec3 result = color.rgb + backgroundColor.rgb * color.a;
    //Cd=Ad*Cbg+Cd;Ad=
    //vec3 result = color.rgb*color.a  + backgroundColor.rgb * (1.0 - color.a);
    FragColor = vec4(result, 1.0);
}
