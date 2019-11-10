#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D texSampler;

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
	//outColor = vec4(1.0f,0.0f,0.0f,1.0f);
	float r =  texture(texSampler, fragTexCoord).r;

	outColor = vec4(vec3(1.0-LinearizeDepth(r)),1.0);
}