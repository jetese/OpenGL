#version 330 core
#define M_PI 3.1416
in vec3 inPos;
in vec3 inColor;
in vec2 inTexCoord;

out vec2 textcoord;

uniform mat4 modelViewProj;
	

void main()
{
	textcoord = inTexCoord;

	gl_Position =  modelViewProj*vec4(inPos,1);
}
//Cambio de base la camara para alejarla del cubo, si quiero ver el cube 1/ tg 30 (-3), Near = 1, r-l = w y cambiamos la formula de la matriz view