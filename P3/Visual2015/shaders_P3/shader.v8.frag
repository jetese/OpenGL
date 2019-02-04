#version 330 core

in vec3 color;
in vec3 NORM;
in vec3 VERT;
in vec2 texCoord;

uniform sampler2D colorTex;
uniform sampler2D emiTex;

out vec4 outColor;

//Propiedades de la luz:
vec3 Ia = vec3(0.35); //Coeficiente ambiental

vec3 Il1 = vec3(1,0,0); //Intensidad de la luz puntual roja
vec3 Il2 = vec3(0,0,1); //Intensidad de la luz puntual azul
vec3 Il3 = vec3(0,1,0); //Intensidad de la luz puntual verde
vec3 Il4 = vec3(0.3,0.2,0.0); //Intensidad de la luz direccional amarilla
vec3 Il5 = vec3(1.0); //Intensidad de la luz focal blanca

//Posiciones de las luces en coodenadas del mundo (poco habitual, se suelen poner en coordenadas de la view)
vec3 Posl1 = vec3(-30,8,20);
vec3 Posl2 = vec3(-50,8,-20);
vec3 Posl3 = vec3(0,8,-10);
vec3 Posl5 = vec3(40,18,2);
vec3 D5 = normalize(vec3(0,-1,0)); //Dirección del foco


//Propiedades del objeto:
vec3 Ka; //Coeficiente ambiental
vec3 Kd; //Coeficiente difuso
vec3 Ks; //Coeficiente especular
vec3 Ke; //Coeficiente emisivo
float n;
vec3 N;
vec3 Pos; //En coordenadas de la cámara


vec3 shade(){
	vec3 c = vec3(0.0);

	//Luz Ambiental:
	c += Ia*Ka;

	//Distancias de las fuentes de luz al vértice:
	float dist1 = length(Posl1 - Pos);
	float dist2 = length(Posl2 - Pos);
	float dist3 = length(Posl3 - Pos);
	float dist5 = length(Posl5 - Pos);
	
	//Factor de atenuación de cada luz:
	float fatt1 = min(1 / (1 + 0.0005 * dist1 + 0.0008 * dist1 * dist1), 1);
	float fatt2 = min(1 / (1 + 0.0005 * dist2 + 0.0008 * dist2 * dist2), 1);
	float fatt3 = min(1 / (1 + 0.0005 * dist3 + 0.0008 * dist3 * dist3), 1);
	float fatt5 = min(1 / (1 + 0.00005 * dist5 + 0.00008 * dist5 * dist5), 1);


	//Luz difusa:
	vec3 L1 = normalize(Posl1 - Pos);
	vec3 L2 = normalize(Posl2 - Pos);
	vec3 L3 = normalize(Posl3 - Pos);
	vec3 L4 = normalize(vec3(-0.5, 1, 0.5)); //Dirección de la luz 4 (direccional)
	vec3 L5 = normalize(Posl5 - Pos);
	D5 = normalize(D5);
	
	
	//Luz especular:
	vec3 V = normalize(-Pos);
	vec3 R1 = normalize(reflect(-L1,N)); //Porque L va en dirección contraria a la luz
	vec3 R2 = normalize(reflect(-L2,N)); 
	vec3 R3 = normalize(reflect(-L3,N));
	vec3 R4 = normalize(reflect(-L4,N));

	
	//Para el foco calculamos la difusa y la especular juntas para aprovechar el mismo "if":
	vec3 Id5; //Intensidad difusa del foco
	float cosAngle = cos(radians(20)); //Apertura del foco
	float sFactor5;

	if (dot(D5,(-L5)) > cosAngle){
		Id5 = pow((dot(D5,-L5) - cosAngle) / (1 - cosAngle), 0.6) * Il5 * fatt5 * dot(L5,N);

		vec3 R5 = normalize(reflect(-L5,N));
		sFactor5 = max(dot(R5,V), 0.0001);
		sFactor5 = fatt5 * pow(sFactor5,n);
	}else{
		Id5 = vec3(0.0);

		sFactor5 = 0;
	}
	 
	vec3 Id = Kd * (Il1*fatt1*dot(L1,N) + Il2*fatt2*dot(L2,N) + Il3*fatt3*dot(L3,N) + Il4*dot(L4,N) + Id5); //Intensidad difusa
	c += clamp(Id, 0,1);
	
	
	float sFactor1 = max(dot(R1,V), 0.0001);
	float sFactor2 = max(dot(R2,V), 0.0001);
	float sFactor3 = max(dot(R3,V), 0.0001);
	float sFactor4 = max(dot(R4,V), 0.0001);
	sFactor1 = fatt1 * pow(sFactor1,n);
	sFactor2 = fatt2 * pow(sFactor2,n);
	sFactor3 = fatt3 * pow(sFactor3,n);
	sFactor4 = pow(sFactor4,n);

	vec3 Is = Ks * (Il1*sFactor1 + Il2*sFactor2 + Il3*sFactor3 + Il4*sFactor4 + Il5*sFactor5); //Intensidad especular

	c += clamp(Is,0,1);

	c += Ke; //Se añade la emisiva

	return c;
}

float fog(){
	float f = 0.00001f;
	float d = length(Pos);

	return exp(-d*d*f);
}

void main()
{	
	//Pasamos las variables a coordenadas de la cámara:
	Posl1 = (view * vec4(Posl1,1)).xyz;
	Posl2 = (view * vec4(Posl2,1)).xyz;
	Posl3 = (view * vec4(Posl3,1)).xyz;
	Posl5 = (view * vec4(Posl5,1)).xyz;
	D5 = normalize((view * vec4(D5,0))).xyz;

	Ka = texture(colorTex,texCoord).xyz;
	Kd = Ka;
	Ks = texture(specularTex, texCoord).xyz;
	Ke = texture(emiTex, texCoord).xyz;

	n = 100.0;
	N = normalize(2 * texture(normalTex, texCoord).xyz - vec3(1.0f)); //Se pasa el intervalo del mapa de normales (0, 1) a (-1, 1) para hacer bump mapping
	Pos = VERT;
	outColor = vec4(shade(), 1.0);
	outColor = mix(vec4(0.0f), outColor, fog()); //Se interpolan los colores en función de la niebla (negro)
}
