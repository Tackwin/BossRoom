#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTextCoord;

out vec3 outColor;
out vec2 outTextCoord;

uniform mat4 t;

void main() {
	gl_Position = t * vec4(aPos.xyz, 1.0);
	outColor = aColor;
	outTextCoord = aTextCoord;
}