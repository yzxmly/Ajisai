#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObject{
	mat4 matToWorld;
	mat4 matToCamera;
	mat4 matToFrustum;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragPosition;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out mat3 matTBN;

void main() {
    gl_Position = ubo.matToFrustum * ubo.matToCamera * ubo.matToWorld * vec4(inPosition, 1.0);
	
	fragTexCoord = inTexCoord;

	fragPosition = vec3(ubo.matToWorld * vec4(inPosition, 1.0));
	
	fragNormal = normalize(mat3(transpose(inverse(ubo.matToWorld))) * inNormal);
	
	vec3 T = normalize(mat3(ubo.matToWorld) * inTangent);
	vec3 B = normalize(mat3(ubo.matToWorld) * inBitangent);
	vec3 N = fragNormal;

	matTBN = mat3(T,B,N);
}