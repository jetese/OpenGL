#version 330 core
#define M_PI 3.1416
in vec3 inPos;

uniform mat4 modelViewProj;
	

void main()
{
	gl_Position =  modelViewProj*vec4(inPos,1);
}
//Cambio de base la camara para alejarla del cubo, si quiero ver el cube 1/ tg 30 (-3), Near = 1, r-l = w y cambiamos la formula de la matriz view