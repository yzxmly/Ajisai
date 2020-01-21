#version 450
#extension GL_ARB_separate_shader_objects : enable

#define PI 3.1415926535897932384626433832795
#define SAMPLES 64

layout(set = 1, binding = 0) uniform sampler2D positionSampler;
layout(set = 1, binding = 1) uniform sampler2D normalSampler;
layout(set = 1, binding = 2) uniform sampler2D diffuseSampler;
layout(set = 1, binding = 3) uniform sampler2D shadowMapSampler;
layout(set = 1, binding = 4) uniform samplerCube cubeMapSampler;
layout(set = 1, binding = 5) uniform sampler2D LTC_MAT;
layout(set = 1, binding = 6) uniform sampler2D LTC_MAG;

layout(set = 0, binding = 0) uniform UniformBufferObject{
	mat4 matToLight;
	mat4 matToFrustum;
	vec4 lightPosition;
	vec4 lightDirection;
	vec4 roughness;
	vec4 lightQuadL0;
	vec4 lightQuadL1;
	vec4 lightQuadL2;
	vec4 lightQuadL3;
} ubo;

const float LUT_SIZE  = 64.0;
const float LUT_SCALE = (LUT_SIZE - 1.0)/LUT_SIZE;
const float LUT_BIAS  = 0.5/LUT_SIZE;
 
layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;


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

	const int numSamples = SAMPLES;
	for(int i = 0 ; i < numSamples ; i++){
		vec2 xi = Hammersley(i, numSamples);
		vec3 microNorm = ImportanceSampleGGX(xi, roughness, norm);
		vec3 toLight = 2 * dot(microNorm, toCam) * microNorm - toCam;

		float NoV = clamp(dot(norm, toCam) , 0.0f ,1.0f);
		float NoL = clamp(dot(norm, toLight), 0.0f, 1.0f);
		float NoH = clamp(dot(norm, microNorm), 0.0f, 1.0f);
		float VoH = clamp(dot(toCam, microNorm), 0.0f, 1.0f);

		if(NoL > 0){
			vec3 sampleColor = texture(cubeMapSampler,toLight).rgb;

			float G = GSmith(roughness, NoV, NoL );
			float Fc = pow(1-VoH, 5);
			vec3 F = (1-Fc) * specular + Fc;

			light += sampleColor * F * G * VoH / (NoH * NoV);
		}
	}
	return light / numSamples;
}

// copy from https://blog.selfshadow.com/sandbox/ltc.html
void ClipQuadToHorizon(inout vec3 L[5], out int n)
{
    // detect clipping config
    int config = 0;
    if (L[0].z > 0.0) config += 1;
    if (L[1].z > 0.0) config += 2;
    if (L[2].z > 0.0) config += 4;
    if (L[3].z > 0.0) config += 8;

    // clip
    n = 0;

    if (config == 0)
    {
        // clip all
    }
    else if (config == 1) // V1 clip V2 V3 V4
    {
        n = 3;
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
        L[2] = -L[3].z * L[0] + L[0].z * L[3];
    }
    else if (config == 2) // V2 clip V1 V3 V4
    {
        n = 3;
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
    }
    else if (config == 3) // V1 V2 clip V3 V4
    {
        n = 4;
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
        L[3] = -L[3].z * L[0] + L[0].z * L[3];
    }
    else if (config == 4) // V3 clip V1 V2 V4
    {
        n = 3;
        L[0] = -L[3].z * L[2] + L[2].z * L[3];
        L[1] = -L[1].z * L[2] + L[2].z * L[1];
    }
    else if (config == 5) // V1 V3 clip V2 V4) impossible
    {
        n = 0;
    }
    else if (config == 6) // V2 V3 clip V1 V4
    {
        n = 4;
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
        L[3] = -L[3].z * L[2] + L[2].z * L[3];
    }
    else if (config == 7) // V1 V2 V3 clip V4
    {
        n = 5;
        L[4] = -L[3].z * L[0] + L[0].z * L[3];
        L[3] = -L[3].z * L[2] + L[2].z * L[3];
    }
    else if (config == 8) // V4 clip V1 V2 V3
    {
        n = 3;
        L[0] = -L[0].z * L[3] + L[3].z * L[0];
        L[1] = -L[2].z * L[3] + L[3].z * L[2];
        L[2] =  L[3];
    }
    else if (config == 9) // V1 V4 clip V2 V3
    {
        n = 4;
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
        L[2] = -L[2].z * L[3] + L[3].z * L[2];
    }
    else if (config == 10) // V2 V4 clip V1 V3) impossible
    {
        n = 0;
    }
    else if (config == 11) // V1 V2 V4 clip V3
    {
        n = 5;
        L[4] = L[3];
        L[3] = -L[2].z * L[3] + L[3].z * L[2];
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
    }
    else if (config == 12) // V3 V4 clip V1 V2
    {
        n = 4;
        L[1] = -L[1].z * L[2] + L[2].z * L[1];
        L[0] = -L[0].z * L[3] + L[3].z * L[0];
    }
    else if (config == 13) // V1 V3 V4 clip V2
    {
        n = 5;
        L[4] = L[3];
        L[3] = L[2];
        L[2] = -L[1].z * L[2] + L[2].z * L[1];
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
    }
    else if (config == 14) // V2 V3 V4 clip V1
    {
        n = 5;
        L[4] = -L[0].z * L[3] + L[3].z * L[0];
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
    }
    else if (config == 15) // V1 V2 V3 V4
    {
        n = 4;
    }
    
    if (n == 3)
        L[3] = L[0];
    if (n == 4)
        L[4] = L[0];
}

float IntegrateEdge(vec3 v1, vec3 v2)
{
    float cosTheta = dot(v1, v2);
    float theta = acos(cosTheta);    
    float res = cross(v1, v2).z * ((theta > 0.001) ? theta/sin(theta) : 1.0);

    return res;
}

vec3 LTC(vec3 specular, float roughness, vec3 objNorm, vec3 toCam, vec3 objPosition){
	vec3 T = normalize(toCam - objNorm*dot(toCam, objNorm));
    vec3 B = cross(objNorm, T);

    // rotate area light in (T1, T2, N) basis

	mat3 MatLocalToWorld = mat3(T, B, objNorm);
	mat3 MatWorldToLocal = transpose(MatLocalToWorld);

	float alpha = acos(dot(objNorm,toCam)) / PI * 2.0f;
	vec2 uv = vec2(roughness, alpha);
    uv = uv*LUT_SCALE + LUT_BIAS;
        
    vec4 t = texture(LTC_MAT, uv);
    mat3 Minv = mat3(
        vec3(  1,   0, t.y),
        vec3(  0, t.z,   0),
        vec3(t.w,   0, t.x)
    );

	vec3 lightQuad0 = vec3(-1.0f,0.1f,0.0f);
	vec3 lightQuad1 = vec3(-1.0f,0.1f,1.0f);
	vec3 lightQuad2 = vec3(-1.0f,2.1f,1.0f);
	vec3 lightQuad3 = vec3(-1.0f,2.1f,0.0f);
	vec3 L[5];
	L[0] = vec3(Minv * MatWorldToLocal * (vec3(ubo.lightQuadL0) - objPosition));
	L[1] = vec3(Minv * MatWorldToLocal * (vec3(ubo.lightQuadL1) - objPosition));
	L[2] = vec3(Minv * MatWorldToLocal * (vec3(ubo.lightQuadL2) - objPosition));
	L[3] = vec3(Minv * MatWorldToLocal * (vec3(ubo.lightQuadL3) - objPosition));

	int n;
	ClipQuadToHorizon(L, n);

	// integrate
	if(n == 0)
		return vec3(0.0f,0.0f,0.0f);

	L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);
    L[4] = normalize(L[4]);
	
	float sum = 0.0;
	sum += IntegrateEdge(L[0], L[1]);
    sum += IntegrateEdge(L[1], L[2]);
    sum += IntegrateEdge(L[2], L[3]);
	
    if (n >= 4)
       	sum += IntegrateEdge(L[3], L[4]);
    if (n == 5)
    	sum += IntegrateEdge(L[4], L[0]);

    sum =abs(sum);

    vec3 Lo_i = vec3(sum, sum, sum);
	Lo_i *= texture(LTC_MAG, uv).w;
	Lo_i /= 2.0f * PI;
	return vec3(specular* Lo_i);
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
	vec3 specular = vec3(181.0f / 256.0f,166.0f / 256.0f,66.0f/ 256.0f) * 2.0f;
	//vec3 specular = vec3(1.0f, 1.0f, 1.0f);
	//vec3 color = SpecularIBL(specular, ubo.roughness.r, objNorm, toCam);
	vec3 color = LTC(specular, ubo.roughness.r,objNorm,toCam, objPosition);

	outColor = vec4(color, 1.0f);

	//vec3 toLight = normalize(lightPosition - objPosition);
	//vec3 h = normalize(toCam + toLight);

	//float NoV = clamp(dot(objNorm, toCam) , 0.0f ,1.0f);
	//float NoL = clamp(dot(objNorm, toLight), 0.0f, 1.0f);
	//float NoH = clamp(dot(objNorm, h), 0.0f, 1.0f);
	//float VoH = clamp(dot(toCam, h), 0.0f, 1.0f);

	//vec3 color = vec3(0.0f);
	//if( NoL > 0){
	//	float roughness = 0.5f;
	//	float alpha = roughness * roughness;
//		float k = (roughness + 1) * (roughness + 1) / 8.0f;
//
//		float d = alpha * alpha / PI / pow((NoH * NoH * (alpha * alpha -1) + 1),2);
//
//		float gv = NoV / (NoV * (1 - k) + k);
//		float gl = NoL / (NoL * (1 - k) + k);
//		float G = gv * gl;
//
//		float Fc = pow(1 - VoH, 5);
//
//		vec3 F = (1 - Fc) * vec3(5.0f , 5.0f , 5.0f) + Fc;
//
//		color = d * F * G / (4.0f * NoV);
//
//	}
//	else{
//		color = vec3(0.0f);
//	}
	

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
	//outColor = vec4(color, 1.0f);
	//outColor = vec4(texture(LTC_MAT,fragTexCoord).rgb,1.0f);

	// calculate transformation matrix for local 
    
   
}