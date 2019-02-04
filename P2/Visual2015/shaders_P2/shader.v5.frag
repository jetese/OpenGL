#version 330 core
//Atenuaci�n

out vec4 outColor;

in vec3 color;
in vec3 NORM;
in vec3 VERT;
in vec2 TexCoord;

uniform mat4 view;
uniform sampler2D colorTex;
uniform sampler2D specularTex;
uniform sampler2D emiTex;

//Propiedades de la luz 1
vec3 Ia1 = vec3(0.3);
vec3 Il1 = vec3(1.0,0,0);
vec3 Posl1 = vec3(0,0,6);//En coordenadas del mundo. Poco habitual

//Propiedades de la luz 2
vec3 Ia2 = vec3(0.0);
vec3 Il2 = vec3(0.0,1.0,0);
vec3 Posl2 = vec3(0,0,-6);

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 Ke;
float n;
vec3 N;
vec3 Pos;//posicion del objeto en coordenadas de la camara

//Funci�n Atenuaci�n
float C1 = 1.00;
float C2 = 0.05;
float C3 = 0.01;
float at = 0;


vec3 shade (vec3 Ia, vec3 Il, vec3 Posl){
	vec3 c = vec3(0.0);

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
	
	//Atenuaci�n
	float d = length(Pos);
	at = min(1.0/(C1+C2*d+C3*d*d),1.0);
	c *= at;

	//Ambiental 
	c += Ia * Ka;
	
	c+= Ke;
	return c;
}

void main()
{
	Posl1 = (view * vec4(Posl1,1)).xyz;
	Posl2 = (view * vec4(Posl2,1)).xyz;

	//Ka = color;
	//Kd = color;
	Ka = texture(colorTex,TexCoord).xyz;
	Kd = Ka;
	Ks = texture(specularTex,TexCoord).xyz;
	n = 100.0f;
	Ke = texture(emiTex,TexCoord).xyz;

	N = normalize(NORM);
	Pos = VERT;

	outColor = vec4(shade(Ia1,Il1,Posl1), 1.0);
	outColor += vec4(shade(Ia2,Il2,Posl2),1.0);   
}
