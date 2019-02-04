#version 330 core

out vec4 outColor;

in vec3 color;
in vec3 NORM;
in vec3 VERT;
in vec2 TexCoord;

uniform mat4 view;
uniform sampler2D colorTex;
uniform sampler2D specularTex;
uniform sampler2D emiTex;

//Pr�piedades de la luz
vec3 Ia = vec3(0.3);
vec3 Il = vec3(1.0);
vec3 Posl = vec3(0,0,6);//En coordenadas del muno. Poco habitual

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 Ke;
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

	c+= Ke;
	return c;
}

void main()
{
	Posl = (view * vec4(Posl,1)).xyz;

	//Ka = color;
	//Kd = color;
	Ka = texture(colorTex,TexCoord).xyz;
	Kd = Ka;
	Ks = texture(specularTex,TexCoord).xyz;
	n = 100.0f;
	Ke = texture(emiTex,TexCoord).xyz;

	N = normalize(NORM);
	Pos = VERT;

	outColor = vec4(shade(), 1.0);   
}