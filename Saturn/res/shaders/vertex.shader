#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTextCoord;

out vec2 outTextCoord;

uniform mat4 origin;
uniform mat4 size;
uniform mat4 rotation;
uniform mat4 translation;

void main() {
	gl_Position = 
		translation * rotation * size * origin * vec4(aPos.xyz, 1.0);
	outTextCoord = aTextCoord;
}