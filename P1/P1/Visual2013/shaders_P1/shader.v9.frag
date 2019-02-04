#version 330 core

out vec4 outColor;


in vec2 textcoord;
in vec3 normalview;

uniform sampler2D colorTex;

void main()
{
	outColor = vec4(normalview, 0);
}
