#version 450 core
in vec2 texCoords;
out vec4 fragColor;

uniform sampler2D tex;

void main()
{
    vec4 output = texture(tex, texCoords);
    
    fragColor = vec4(output.rgb, 1.0);
    
    // fragColor = vec4(value.rrr, 1.0);
}