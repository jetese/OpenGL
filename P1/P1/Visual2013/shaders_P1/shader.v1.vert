#version 330 core
#define M_PI 3.1416
in vec3 inPos;

mat4 proj;
mat4 model;
mat4 view;		

void main()
{
	model = mat4(1.0); 
	view = mat4(1.0);
	view[3].z = -3;

	proj = mat4(1.0);
	float f = 1.0/tan(M_PI/6);
	float near=1;
	float far = 10;

	proj[0].x = f;
	proj[1].y = f;
	proj[2].z = (far + near) / (near-far);
	proj[2].w = -1;
	proj[3].z = (2*far*near) /(near-far);
	proj[3].w = 0;

	gl_Position =  proj*view*model*vec4(inPos,1);
}
//Cambio de base la camara para alejarla del cubo, si quiero ver el cube 1/ tg 30 (-3), Near = 1, r-l = w y cambiamos la formula de la matriz view