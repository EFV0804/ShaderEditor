#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;


layout(set = 0, binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} camera;

//push constants block
layout( push_constant ) uniform PushConstants
{
    mat4 transform;
} constants;


void main() {
    gl_Position = (camera.viewproj * constants.transform) * vec4(inPosition, 1.0);
    fragColor = inColor;
}
