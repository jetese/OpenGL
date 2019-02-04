#version 330 core
//Uniformes
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform mat4 normal;
uniform mat4 modelView;
uniform mat4 modelViewProj;

//Texturas
uniform sampler2D colorTex;
uniform sampler2D emiTex;
uniform sampler2D normalTex;
uniform sampler2D specularTex;
uniform sampler2D auxiliarTex;

out vec4 outColor;


void main()
{
	outColor = vec4(1.0);   
}
