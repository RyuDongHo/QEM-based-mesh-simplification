#version 330 core

in vec4 vColor;
in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D textureSampler;
uniform bool useTexture;

void main(void){
	if (useTexture) {
		fragColor = texture(textureSampler, vTexCoord) * vColor;
	} else {
		fragColor = vColor;
	}
}