#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D texSampler_diffuse;
layout(set = 1, binding = 1) uniform sampler2D texSampler_specular;
layout(set = 1, binding = 2) uniform sampler2D texSampler_normal;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragPosition;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 camPos;
layout(location = 4) in mat3 matTBN;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outDiffuse;

void main() {
	vec3 direction = normalize(vec3(-0.5f,-0.1f,-0.5f));
	vec3 lightColor = vec3(1.0f,1.0f,1.0f);

	vec3 normal = normalize(texture(texSampler_normal, fragTexCoord).rgb);
	normal = normalize(matTBN * normal);

	// diffuse color
	float intensity = max(dot(normal, -direction), 0.0);
	vec3 colorDiffuse = intensity * lightColor * vec3(texture(texSampler_diffuse, fragTexCoord));

	// specular color
	vec3 camDirection = normalize(camPos-fragPosition);
	vec3 halfwayDirection = normalize(camDirection - direction);

	float specIntensity = pow(max(dot(halfwayDirection, normal), 0.0), 32);
	vec3 colorSpecular = intensity * lightColor * vec3(texture(texSampler_specular, fragTexCoord));
	
	outPosition = vec4(fragPosition, 1.0);
	//outPosition = vec4(normal, 1.0);
	outNormal = vec4(fragNormal, 1.0);
	outDiffuse = texture(texSampler_diffuse, fragTexCoord);

}