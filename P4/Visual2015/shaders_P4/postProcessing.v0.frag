#version 330 core

//Color de salida
out vec4 outColor;
//Variables Variantes
in vec2 texCoord;
//Textura
uniform sampler2D colorTex;


void main()
{
	//Código del Shader
	outColor = vec4(textureLod(colorTex, texCoord,0).xyz, 0.6);
	//outColor = vec4(texCoord,vec2(1.0));
}