#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

// Transformation matrices.
uniform mat4 worldMatrix;
uniform mat4 normalMatrix;
uniform mat4 MVP;

// Uniforms for animation.
uniform float TIME;          // Animation time
uniform float time_scale;    // Time scaling factor
uniform float side_to_side;  // Side-to-side motion magnitude
uniform float pivot;         // Pivot motion magnitude
uniform float wave;          // Wave motion magnitude
uniform float twist;         // Body Twist

// Data passed to fragment shader.
out vec3 fsPosition;
out vec3 fsNormal;
out vec2 iTexCoord; //new

void main()
{
    float time = TIME * time_scale;
    vec3 animatedPosition = Position;

    // Side-to-side motion
    animatedPosition.x += cos(time) * side_to_side;

    // Pivot rotation
    float pivot_angle = cos(time) * 0.1 * pivot;
    mat2 rotation_matrix = mat2(vec2(cos(pivot_angle), -sin(pivot_angle)), vec2(sin(pivot_angle), cos(pivot_angle)));
    animatedPosition.xz = rotation_matrix * animatedPosition.xz;

    // Calculate body position for wave motion
    float body = (animatedPosition.z + 1.0) / 1.5; // Assuming fish centered at (0, 0) with a length of 2

    // Wave motion along the spine
    animatedPosition.x += cos(time + body) * wave;

    // Twist motion
    float twist_angle = cos(time + body) * 0.3 * twist;
    mat2 twist_matrix = mat2(vec2(cos(twist_angle), -sin(twist_angle)), vec2(sin(twist_angle), cos(twist_angle)));

    animatedPosition.xy = twist_matrix * animatedPosition.xy;

    // Applying transformations

    vec4 worldPos = worldMatrix * vec4(animatedPosition, 1.0);
    gl_Position = MVP * vec4(animatedPosition, 1.0);
    iTexCoord = TexCoord; //new
    
    fsPosition = worldPos.xyz / worldPos.w;
    fsNormal = (normalMatrix * vec4(Normal, 0.0)).xyz;
}
