#version 330 core

in vec3 inPos;	
in vec3 inColor;
in vec3 inNormal;

uniform mat4 modelViewProj;
uniform mat4 view;
uniform mat4 normal;
uniform mat4 modelView;

out vec3 color;

//Propiedades de la luz
vec3 Ia = vec3(0.3);
vec3 Il = vec3(1.0);
vec3 Posl = vec3(0,0,6);//En coordenadas del muno. Poco habitual

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
float n;
vec3 N;
vec3 Pos;//posicion del objeto en coordenadas de la camara

vec3 shade (){
	vec3 c = vec3(0.0);

	//Ambiental 
	c += Ia * Ka;

	//Difusa
	vec3 L = normalize(Posl - Pos);
	vec3 Id = Il * Kd * dot(L,N);
	c += clamp (Id, 0, 1);//Si es menor que 0 pone 0, si es mayor que 1 pone 1
	
	//Especular
	vec3 V = normalize(-Pos);
	vec3 R = normalize(reflect(-L,N));
	float sfactor = max(dot (R,V), 0.0001);
	sfactor = pow (sfactor, n);
	vec3 Is = Il*Ks*sfactor;
	c += clamp(Is,0,1);
	return c;
}

void main()
{
	/*Ka = inColor;
	Kd = Ka;*/
	Ka = vec3(1,vec2(0));
	Kd = vec3(1,vec2(0));
	Ks = vec3(1.0);
	n = 20.0f;

	N = normalize((normal*vec4(inNormal,0)).xyz);
	Posl = (view * vec4 (Posl,1)).xyz;
	Pos = (modelView * vec4(inPos,1)).xyz;

	color = shade();
	gl_Position =  modelViewProj * vec4 (inPos,1.0);
}
