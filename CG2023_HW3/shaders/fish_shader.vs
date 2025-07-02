#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in mat4 InstanceMatrix;


uniform mat4 MVP;

// Data passed to fragment shader.
out vec3 fsPosition;
out vec3 fsNormal;
out vec2 iTexCoord; //new


void main()
{

    gl_Position = MVP * InstanceMatrix * vec4(Position, 1.0);

    iTexCoord = TexCoord; 
    
    mat4 worldMatrix = InstanceMatrix;

    mat4 normalMatrix = transpose(inverse(worldMatrix));

    vec4 worldPos = worldMatrix * vec4(Position, 1.0);

    fsPosition = worldPos.xyz / worldPos.w;

    fsNormal = (normalMatrix * vec4(Normal, 0.0)).xyz;
}
