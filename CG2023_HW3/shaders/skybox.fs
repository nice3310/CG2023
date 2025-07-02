#version 330 core

in vec2 iTexCoord;

// Material properties.
uniform sampler2D mapKd;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(mapKd, vec2(iTexCoord.x, 1.0 - iTexCoord.y));
    FragColor = vec4(vec3(0.5f,0.5f,0.5f) * texColor.rgb, texColor.a);

}
