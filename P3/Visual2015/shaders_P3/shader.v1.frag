#version 330 core

out vec4 outColor;

in vec3 color;
in vec3 pos;
in vec3 norm;
in vec2 texCoord;
in vec3 tangent;

uniform mat4 View;
uniform vec3 incLightIntensity;
uniform vec3 myLightPos;
uniform sampler2D colorTex;
uniform sampler2D emiTex;
uniform sampler2D specTex;
uniform sampler2D normalTex;

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 N;
float alpha = 100.0;
vec3 Ke;

//Propiedades de la luz
vec3 Ia = vec3 (0.3);
vec3 Id = incLightIntensity;
vec3 Is = incLightIntensity;
vec3 lpos = (View*vec4(myLightPos,1)).xyz;

//Propiedades luz direccional
vec3 IaLD = vec3(0.1);
vec3 IlLD = vec3(0.5,0.5,0.0);
vec3 PoslLD = (View*vec4(10,0,0,1)).xyz;

//Propiedades de la luz focal 4
vec3 IaLF = vec3(0.0);
vec3 IlLF =  vec3(1.0,0,0);
vec3 PoslLF = (View * vec4(5,0,0,1)).xyz;
float aper = cos(radians(10));
float Expfoc = 3;
vec3 DirLF = (View * vec4(-1,0,0,0)).xyz;

//Función Atenuación
float C1 = 1.00;
float C2 = 0.05;
float C3 = 0.01;
float at = 0;

vec3 shade();
vec3 shadeDirectionalLight();
vec3 shadeFocalLight();

mat3 buildtbn(){
	vec3 N = normalize (norm);
	vec3 T = tangent;

	T = normalize(T- dot(T,N)*N);
	vec3 b = cross (N,T);

	return transpose(mat3(T,b,N));
}

void main()
{
	Ka = texture(colorTex, texCoord).rgb;
	Kd = texture(colorTex, texCoord).rgb;
	Ke = texture(emiTex, texCoord).rgb;
	Ks = texture(specTex,texCoord).rgb;

	N = normalize (2*texture(normalTex,texCoord).xyz-vec3(1.0));
	
	outColor = vec4(shade(), 1.0);   

	outColor += vec4(shadeFocalLight(),0); 
	
	N= normalize(norm);
	outColor += vec4(shadeDirectionalLight(),1.0);
	
}

vec3 shade()
{
	
	vec3 c = vec3(0.0);
	mat3 tbn = buildtbn();

	c = Ia * Ka;

	vec3 L = normalize ((lpos - pos)*tbn);
	vec3 diffuse = Id * Kd * dot (L,N);
	c += clamp(diffuse, 0.0, 1.0);
	
	vec3 V = normalize (-pos*tbn);
	vec3 R = normalize (reflect (-L,N));
	float factor = max (dot (R,V), 0.0001);
	vec3 specular = Is*Ks*pow(factor,alpha);
	c += clamp(specular, 0.0, 1.0);

	//Atenuación
	float d = length(pos);
	at = min(1.0/(C1+C2*d+C3*d*d),1.0);
	c *= at;


	c+=Ke;
	
	return c;
}

vec3 shadeDirectionalLight(){
	vec3 c = vec3(0.0);
	//Difusa
	vec3 L = normalize(pos-PoslLD); //distancia
	vec3 Id = IlLD * Kd * max(dot(L,N),0);
	c += clamp (Id, 0, 1); //Si es menor que 0 pone 0, si es mayor que 1 pone 1

	//Especular
	vec3 V = normalize(-pos);
	vec3 R = normalize(reflect(-L,N));
	float sfactor = max(dot (R,V), 0.0001);
	sfactor = pow(sfactor, alpha);
	vec3 Is = IlLD*Ks*sfactor;
	c += clamp(Is,0,1);


	//Ambiental 
	c += IaLD * Ka;
	c += Ke;
	return c;
}

vec3 shadeFocalLight(){
	vec3 c = vec3(0.0);
	float Dotfocal  = dot(normalize(pos - PoslLF),normalize( DirLF));
	
	vec3 L = normalize(PoslLF-pos);
	vec3 Id = IlLF * Kd * dot(L,N);
	vec3 trunc = clamp (Id, 0, 1);//Si es menor que 0 pone 0, si es mayor que 1 pone 1
	

	//Especular
	vec3 V = normalize(-pos);
	vec3 R = normalize(reflect(-L,N));
	float sfactor = max(dot (R,V), 0.0001);
	sfactor = pow (sfactor, alpha);
	vec3 Is = IlLF*Ks*sfactor;
	vec3 e = clamp(Is,0,1);

	if (Dotfocal > aper)
		c+= trunc+e*pow((Dotfocal-aper)/(1-aper),Expfoc);
		
	//Atenuación
	float d = length(pos);
	at = min(1.0/(C1+C2*d+C3*d*d),1.0);
	c *= at;

	//Ambiental 
	c += IaLF * Ka;
	
	c+= Ke;
	return c;
}

