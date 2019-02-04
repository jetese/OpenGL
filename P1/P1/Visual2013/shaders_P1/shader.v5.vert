#version 330 core
#define M_PI 3.1416
in vec3 inPos;

out vec3 color;

uniform mat4 modelViewProj;
	

void main()
{
	if (mod(gl_VertexID,2)==0)
		color = vec3(0,0,1);
	else
		color = vec3(1,0,0);

	gl_Position =  modelViewProj*vec4(inPos,1);
}
//Cambio de base la camara para alejarla del cubo, si quiero ver el cube 1/ tg 30 (-3), Near = 1, r-l = w y cambiamos la formula de la matriz view