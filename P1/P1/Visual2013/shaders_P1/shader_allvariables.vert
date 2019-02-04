#version 330 core

//Atributos
in vec3 inPos;		
in vec3 inColor;	
in vec3 inNormal;	
in vec2 inTexCoord;	
in vec3 inTangent;	

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

void main()
{
	gl_Position =  modelViewProj * vec4(inPos, 1);
}
