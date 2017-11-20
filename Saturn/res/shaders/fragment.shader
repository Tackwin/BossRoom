#version 330 core

in vec3 outColor;
in vec2 outTextCoord;

out vec4 FragColor;

uniform sampler2D myTexture;

void main() {
	FragColor = texture(myTexture, outTextCoord);
}