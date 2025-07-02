#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in mat4 InstanceMatrix;

out vec2 iTexCoord;

uniform mat4 MVP;

void main() {

    gl_Position = MVP * InstanceMatrix * vec4(Position, 1.0);
    iTexCoord = TexCoord;

}