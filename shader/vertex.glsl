#version 330 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec4 vColor;
out vec2 vTexCoord;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

void main(void){
	gl_Position = projMat * viewMat * modelMat * aPos;
	vColor = aColor;
	vTexCoord = aTexCoord;
}