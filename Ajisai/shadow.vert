#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObject{
	mat4 matToWorld;
	mat4 matToLight;
	mat4 matToFrustum;
	vec4 lightPosition;
	vec4 lightDirection;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;



void main() {
    gl_Position = ubo.matToFrustum * ubo.matToLight * ubo.matToWorld * vec4(inPosition, 1.0);
}