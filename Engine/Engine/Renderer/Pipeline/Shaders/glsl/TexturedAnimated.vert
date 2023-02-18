#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 pose[120];
    mat4 invBindPose[120];
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inWeights;
layout(location = 4) in ivec4 inJoints;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragUVCoord;

void main(void) 
{
     mat4 skin = (ubo.pose[inJoints.x] * ubo.invBindPose[inJoints.x]) * inWeights.x;
     skin += (ubo.pose[inJoints.y] * ubo.invBindPose[inJoints.y]) * inWeights.y;
     skin += (ubo.pose[inJoints.z] * ubo.invBindPose[inJoints.z]) * inWeights.z;
     skin += (ubo.pose[inJoints.w] * ubo.invBindPose[inJoints.w]) * inWeights.w;

     gl_Position = ubo.projection * ubo.view * ubo.model * skin * vec4(inPosition, 1.0);
     fragNormal = inNormal;
     fragUVCoord = inUV;
}