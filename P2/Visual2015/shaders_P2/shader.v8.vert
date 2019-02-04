#version 330 core

in vec3 inPos;	
in vec3 inColor;
in vec3 inNormal;
in vec2 inTexCoord;
in vec3 inTangent;	

uniform mat4 modelViewProj;
uniform mat4 view;
uniform mat4 normal;
uniform mat4 modelView;

out vec3 color;
out vec3 NORM;
out vec3 VERT;
out vec2 TexCoord;
out vec3 TANG;

void main()
{
	NORM = (normal*vec4(inNormal,0)).xyz;
	VERT = (modelView * vec4(inPos,1)).xyz;
	TANG = (modelView * vec4(inTangent,0)).xyz;

	color = inColor;
	TexCoord = inTexCoord;
	gl_Position =  modelViewProj * vec4 (inPos,1.0);
}
