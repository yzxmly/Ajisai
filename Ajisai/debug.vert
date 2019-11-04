#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;

layout(location = 0) out vec2 fragTexCoord;


void main() {
    gl_Position = vec4(inPosition, 1.0);
	
	fragTexCoord = (vec2(inPosition) + 1.0) / 2.0;
}