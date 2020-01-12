#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D positionSampler;
layout(set = 1, binding = 1) uniform sampler2D normalSampler;
layout(set = 1, binding = 2) uniform sampler2D diffuseSampler;
layout(set = 1, binding = 3) uniform sampler2D shadowMapSampler;

layout(set = 0, binding = 0) uniform UniformBufferObject{
	mat4 matToLight;
	mat4 matToFrustum;
	vec4 lightPosition;
	vec4 lightDirection;
} ubo;
 
layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

float LinearizeDepth(float depth)
{
  float n = 1.0; // camera z near
  float f = 100.0; // camera z far
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));	
}

void main() {
	// for now, hard code light information here
	vec4 pos = vec4(texture(positionSampler, fragTexCoord).rgb , 1.0f);
	vec4 lightPos = (ubo.matToFrustum * ubo.matToLight * pos);
	vec3 lightFrustumPos = lightPos.xyz / lightPos.w;
	vec3 projCoords = lightFrustumPos.xyz * 0.5 + 0.5;
	float closestDepth = texture(shadowMapSampler, projCoords.xy).r;
	float currentDepth = lightFrustumPos.z;
	float atten = 0.0f;
	if (currentDepth - 0.02f  < closestDepth) atten = 1.0f;
	
	//outColor = vec4(vec3(currentDepth), 1.0f);
	outColor = vec4(vec3(closestDepth), 1.0f);
	//outColor = vec4(vec3(texture(shadowMapSampler, fragTexCoord).r), 1.0f);
	outColor = vec4(atten, 0.0f,0.0f, 1.0f);
	outColor = vec4(atten* texture(diffuseSampler,fragTexCoord). xyz, 1.0f);
}