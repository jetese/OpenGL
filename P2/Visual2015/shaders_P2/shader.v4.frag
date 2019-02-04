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

//Propiedades de la luz 1
vec3 Ia1 = vec3(0.3);		//Intensidad Ambiental
vec3 Il1 = vec3(0,1.0,0);		//Intensidad luminica
vec3 Posl1 = vec3(0,0,6);	//Posición de la luz en coordenadas del mundo ( poco habitual)

//Propiedades de la luz 2
vec3 Ia2 = vec3(0.3);
vec3 Il2 = vec3(1.0,0,0);
vec3 Posl2 = vec3(-6.0,0,0);

//Propiedades del objeto
vec3 Ka;					//Coeficiente de reflexión ambiental			
vec3 Kd;					//Coeficiente de reflexión difuso
vec3 Ks;					//Coeficiente de reflexión especular
vec3 Ke;					//Coeficiente de colores de la textura
float n;					//Indice que simula la rugosidad de la superficie
vec3 N;						//Normal de la superficie en el punto P
vec3 Pos;					//Posición del objeto en coordenadas de la camara

vec3 shade(vec3 Ia, vec3 Il, vec3 Posl){
	vec3 c = vec3(0.0);

	//Luz ambiental	= Iambiental * Coeficiente de reflexión ambiental

	c += Ia * Ka;		

	//Luz difusa = Intensidad lumínica * Coeficiente de reflexión difusa 
	//				* (Normal del punto (producto escalar) Vector de incidencia de la luz) * fatt(1)

	vec3 L = normalize(Posl-Pos);	
	vec3 Id = Il * Kd * dot(L,N);
	c+= clamp(Id,0,1);

	//Luz especular = Il * Ks *dot (R,V) ^ n
	// R = 2 * dot(vector normales, vector direccion luz) * vector normales - vector direccion Luz
	
	vec3 V = normalize(-Pos);				// Vector de vista (Posición del objeto - posición de la camara)
	vec3 R = normalize(reflect(-L,N));		// Reflect =  -L - 2.0 * dot(N, -L) * N = R
	float sfactor = max(dot (R,V), 0.0001);	// No entiendo porque el máximo de dot(R,V),0.0001
	sfactor = pow (sfactor, n);				// dot(R,V) ^ n
	vec3 Is = Il*Ks*sfactor;				// Il * Ks * dot(R,V) ^ n
	c += clamp(Is,0,1);						// Luz ambiental + Luz difusa + Luz especular

	c+= Ke;									//Color de la textura

	return c;

}

void main()
{
	Posl1 = (view * vec4 (Posl1,1)).xyz;			// Transformar la posición de la luz en coordenadas del
	Posl2 = (view * vec4 (Posl2,1)).xyz;			// mundo a coordenadas de la camara
													

	Ka = texture(colorTex,TexCoord).xyz;			//Coeficiente de reflexion ambiental = textura

	Kd = Ka;										//Coeficiente de reflexion difusa = Color de cada vertice

	Ks = texture(specularTex,TexCoord).xyz;			//Coeficiente de reflexion especular =  textura					

	Ke = texture(emiTex,TexCoord).xyz;				//Textura emisiva								

									
	n = 100.0f;										// Coeficiente de rugosidad para reflexion especular

	N = normalize(NORM);							//Normalizacion de las normales de los vertices en coordenadas de la camara
	Pos = VERT;										//Posicion de cada vertices en coordenada de la camara

	outColor = vec4(shade(Ia1,Il1,Posl1), 1.0);					//Calculo de los fragmentos
	outColor += vec4(shade(Ia2,Il2,Posl2),1.0);
}