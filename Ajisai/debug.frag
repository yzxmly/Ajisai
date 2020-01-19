#version 450
#extension GL_ARB_separate_shader_objects : enable

#define PI 3.1415926535897932384626433832795

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

// since sum D(h) > 1 and sum D(h) h . n = 1
// the improtance sampler uses the distribution D(h) h . n
vec3 ImportanceSampleGGX(vec2 xi, float roughness, vec3 norm){
	float a = roughness * roughness;
	float phi = 2 * PI * xi.x;
	float cosTheta = sqrt((1-xi.y) / (1+ (a*a -1) * xi.y));
	float sinTheta = sqrt(1-cosTheta*cosTheta);

	vec3 upVector = abs(norm.z) < 0.999 ? vec3(0,0,1.0f) : vec3(1.0f,0.0f,0.0f);
	vec3 tangent = normalize(cross(upVector, norm));
	vec3 biTangent = cross(norm, tangent);

	return sinTheta * cos(phi) * tangent + sinTheta * sin (phi) * biTangent +  cosTheta * norm;
}

 float radicalInverse_VdC(uint bits) {
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
 }

vec2 Hammersley(uint i , uint numSamples){
	return vec2(float(i)/float(numSamples), radicalInverse_VdC(i));
}

float GSmith(float roughness, float NoV, float NoL){
	float k = (roughness + 1) * (roughness + 1) / 8;
	float gv = NoV / (NoV * (1-k) + k );
	float gl = NoL / (NoL * ( 1 - k ) + k );
	return gv * gl;
}

// the integral domain changes from incident light l  to normal of microfacet h (mean of indicdent light and camera direction)
// Jacobian is 1 / (4 v * h) v is camera directon 
// sample based on D(h) h.n, thus parameter is  h . n / 4 / v.h
vec3 SpecularIBL(vec3 specular, float roughness, vec3 norm, vec3 toCam){
	vec3 light = vec3(0.0f);

	const int numSamples = 16;
	for(int i = 0 ; i < numSamples ; i++){
		vec2 xi = Hammersley(i, numSamples);
		vec3 microNorm = ImportanceSampleGGX(xi, roughness, norm);
		vec3 toLight = 2 * dot(microNorm, toCam) * microNorm - toCam;

		float NoV = clamp(dot(norm, toCam) , 0.0f ,1.0f);
		float NoL = clamp(dot(norm, toLight), 0.0f, 1.0f);
		float NoH = clamp(dot(norm, microNorm), 0.0f, 1.0f);
		float VoH = clamp(dot(toCam, microNorm), 0.0f, 1.0f);

		if(NoL > 0){
			vec3 sampleColor = vec3(1.0f);

			float G = GSmith(roughness, NoV, NoL );
			float Fc = pow(1-VoH, 5);
			vec3 F = (1-Fc) * specular + Fc;

			light += sampleColor * F * G * VoH / (NoH * NoV);
		}
	}
	return light;
}

void main() {
	// implement the ggx metal shading
	// right now just hard code the light position

	// hard code the light the light is a triangle
	vec3 camPosition = vec3(15.0f, 15.0f, 15.0f);
	vec3 objPosition = texture(positionSampler, fragTexCoord).rgb;
	vec3 objNorm = normalize(texture(normalSampler,fragTexCoord).rgb);
	vec3 lightPosition = vec3(ubo.lightPosition);

	vec3 toCam = normalize(camPosition - objPosition);
	vec3 toLight = normalize(lightPosition - objPosition);
	vec3 h = normalize(toCam + toLight);

	float NoV = clamp(dot(objNorm, toCam) , 0.0f ,1.0f);
	float NoL = clamp(dot(objNorm, toLight), 0.0f, 1.0f);
	float NoH = clamp(dot(objNorm, h), 0.0f, 1.0f);
	float VoH = clamp(dot(toCam, h), 0.0f, 1.0f);

	vec3 color = vec3(0.0f);
	if( NoL > 0){
		float roughness = 0.4f;
		float alpha = roughness * roughness;
		float k = (roughness + 1) * (roughness + 1) / 8.0f;

		float d = alpha * alpha / PI / pow((NoH * NoH * (alpha * alpha -1) + 1),2);

		float gv = NoV / (NoV * (1 - k) + k);
		float gl = NoL / (NoL * (1 - k) + k);
		float G = gv * gl;

		float Fc = pow(1 - VoH, 5);

		vec3 F = (1 - Fc) * vec3(5.0f , 5.0f , 5.0f) + Fc;

		color = d * F * G / (4.0f * NoV);

	}
	else{
		color = vec3(0.0f);
	}
	

	//vec4 posWorld = vec4(texture(positionSampler, fragTexCoord).rgb , 1.0f);
	//vec4 posLight = (ubo.matToFrustum * ubo.matToLight * posWorld);
	//vec3 posLightFrustum = posLight.xyz / posLight.w;
	//vec3 shadowTexCoord = posLightFrustum.xyz * 0.5 + 0.5;
	//float closestDepth = texture(shadowMapSampler, shadowTexCoord.xy).r;
	//float currentDepth = posLightFrustum.z;
	//float shadowAtten = 0.0f;
	//if (currentDepth - 0.02f  < closestDepth) shadowAtten = 1.0f;

	//vec3 normWorld = normalize(texture(normalSampler, fragTexCoord).rgb);

	//vec3 toLight = normalize(ubo.lightPosition.rgb - posWorld.rgb);
	//float diffuseAtten = max(0, dot(toLight, normWorld));

	//outColor = vec4(shadowAtten * diffuseAtten * vec3(1.0f,1.0f,1.0f), 1.0f);
	//outColor = vec4(texture(normalSampler,fragTexCoord).rgb, 1.0f);
	outColor = vec4(color, 1.0f);
}