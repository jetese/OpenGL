#version 330 core

out vec4 outColor;
in vec2 textcoord;

uniform sampler2D colorTex;

void main()
{
	outColor = texture(colorTex,textcoord);
}
