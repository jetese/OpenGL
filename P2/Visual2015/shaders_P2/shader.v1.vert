#version 330 core

in vec3 inPos;	
in vec3 inColor;
in vec3 inNormal;

uniform mat4 modelViewProj;
uniform mat4 modelView;
uniform mat4 normal;
uniform mat4 view;

out vec3 color;

//Propiedades de la luz
vec3 Ia = vec3(0.3);		//Intensidad Ambiental
vec3 Il = vec3(1.0);		//Intensidad luminica
vec3 Posl = vec3(0,0,6);	//Posición de la luz en coordenadas del mundo ( poco habitual)

//Propiedades del objeto
vec3 Ka;					//Coeficiente de reflexión ambiental			
vec3 Kd;					//Coeficiente de reflexión difuso
vec3 Ks;					//Coeficiente de reflexión especular
float n;					//Indice que simula la rugosidad de la superficie
vec3 N;						//Normal de la superficie en el punto P
vec3 Pos;					//Posición del objeto en coordenadas de la camara

vec3 shade(){
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
	return c;

}


void main()
{
	//Coeficientes de reflexión y de rugosidad de la superficie para reflexion especular
	Ka = vec3(1,vec2(0)); //Color rojo
	Kd = vec3(1,vec2(0));	//Color rojo
	Ks = vec3(1.0);
	n = 20.0f;

	
	N = normalize((normal*vec4(inNormal,0)).xyz);	// Transformar las normales asociadas a los vertices de 
													// Coordenadas del modelo a coordenadas de la cámara

	Posl = (view * vec4 (Posl,1)).xyz;				// Transformar la posición de la luz en coordenadas del
													// mundo a coordenadas de la camara
													 
	Pos = (modelView * vec4(inPos,1)).xyz;			// Transforma los vertices del objeto de coordenadas del
													//	modelo a coordenadas de la camara

	color = shade();								// Calculamos el color de cada vertice según las luces

	gl_Position =  modelViewProj * vec4 (inPos,1.0);// Transforma la posicion de los vertices de coordenadas
													// del objeto a coordenadas del plano de proyección
}
