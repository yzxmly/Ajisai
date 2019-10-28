#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D texSampler_diffuse;
layout(set = 1, binding = 1) uniform sampler2D texSampler_specular;
layout(set = 1, binding = 2) uniform sampler2D texSampler_normal;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragPosition;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in mat3 matTBN;

layout(location = 0) out vec4 outColor;

void main() {
	vec3 direction = normalize(vec3(-0.5,-0.5f,-0.5f));
	vec3 lightColor = vec3(1.0f,1.0f,1.0f);

	vec3 normal = texture(texSampler_normal, fragTexCoord).rgb;
	normal = normalize(matTBN * normal);

	float intensity = max(dot(normal, -direction), 0.0);
	vec3 colorDiffuse = intensity * lightColor * vec3(texture(texSampler_diffuse, fragTexCoord));
	
    //outColor = texture(texSampler_specular, fragTexcoord * 2.0);
	outColor = vec4(colorDiffuse,1.0f);
}