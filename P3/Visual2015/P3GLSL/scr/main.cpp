#include "BOX.h"
#include "auxiliar.h"

#include <windows.h>

#include <gl/glew.h>
#include <gl/gl.h>
#define SOLVE_FGLUT_WARNING
#include <gl/freeglut.h> 
#include <iostream>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define M_PI 3.1415
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

//////////////////////////////////////////////////////////////
// Datos que se almacenan en la memoria de la CPU
//////////////////////////////////////////////////////////////

//Matrices
glm::mat4	proj = glm::mat4(1.0f);
glm::mat4	view = glm::mat4(1.0f);
glm::mat4	model = glm::mat4(1.0f);
glm::mat4	model2 = glm::mat4(1.0f);
glm::mat4	model3 = glm::mat4(1.0f);

glm::vec3	lightintensity = glm::vec3(1.0f);
glm::vec3	lightpos = glm::vec3(0.0, 0.0, 5.0);

//Tangentes y normales
float tangent[72];
float lolnormal[72];

//Variables Assimp
const int numVertex = 100000;
const int numFaces = 100000;
Assimp::Importer importer;
const aiScene* scene;

int miNumVertex;
int miNumFaces;

unsigned int miVertexIndex[numFaces * 3];
float miVertexPos[numVertex * 3];
float miModelColor[numVertex * 3];
float miNormals[numVertex * 3];
float miTexCoord[numVertex * 3];;
float miVertexTang[numVertex * 3];

//////////////////////////////////////////////////////////////
// Variables que nos dan acceso a Objetos OpenGL
//////////////////////////////////////////////////////////////
//Por definir

unsigned int vshader; //Id vertices
unsigned int fshader; //Id fragmentos 
unsigned int program; //Id Programa que vamos a compilar

unsigned int vshader2;
unsigned int fshader2;
unsigned int program2;

					  //Variables Uniform 
int uModelViewMat;
int uModelViewProjMat;
int uNormalMat;
int uIncLightIntensity;
int uMyLightPos;
int uColorTex;
int uEmiTex;
int uSpecTex;
int uNormalTex;
int uView;
int uModelViewMat2;
int uModelViewProjMat2;
int uNormalMat2;
int uColorTex2;
int uEmiTex2;




//Atributos 
int inPos;
int inColor;
int inNormal;
int inTexCoord;
int inTangent;
int inPos2;
int inColor2;
int inNormal2;
int inTexCoord2;


//Texturas
unsigned int colorTexId;
unsigned int emiTexId;
unsigned int specTexId;
unsigned int normalTexId;
unsigned int tex2;
unsigned int tex3;
unsigned int verde;
unsigned int colorTexId2;
unsigned int emiTexId2;


//VAO
unsigned int vao;
unsigned int vao2;
unsigned int vao3;
//VBOs que forman parte del objeto
unsigned int posVBO;
unsigned int colorVBO;
unsigned int normalVBO;
unsigned int texCoordVBO;
unsigned int tangentVBO;
unsigned int triangleIndexVBO;

//unsigned int myposVBO;
//unsigned int mycolorVBO;
//unsigned int mynormalVBO;
//unsigned int mytexCoordVBO;
//unsigned int mytriangleIndexVBO;

float t = 0;//tiempo para la curva de bezier
float c = 0.0001;


//////////////////////////////////////////////////////////////
// Funciones auxiliares
//////////////////////////////////////////////////////////////
//!!Por implementar

//Declaración de CB 
void renderFunc();
void renderAssimp();
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);

//Funciones de inicialización y destrucción
void initContext(int argc, char** argv);//Inicialización del contexto
void initOGL();//Inicialización de cauce, se puede mover del sitio
void initShader(const char *vname, const char *fname);//Inicialicación de shaderss
void initShader2(const char *vname, const char *fname);
void initObj();//Inicialización del objeto que vamos a pintar
void destroy();//liberar los recursos

			   //Funciones para calcular tangentes y normales
void generatetangent(glm::vec3 v0, glm::vec3 v1, glm::vec2 st1, glm::vec2 st2, int i);
void generatetangent(glm::vec3 v0, glm::vec3 v1, glm::vec2 st1, glm::vec2 st2, int i, unsigned int indvertices[]);
void sacarpuntos();
void sacarpuntos(float posvertices[], unsigned int indvertices[], float texcoord[], int numcaras);
void generatenormal(glm::vec3 v0, glm::vec3 v1, int i, int k);


//Carga el shader indicado, devuele el ID del shader
//!Por implementar
GLuint loadShader(const char *fileName, GLenum type);

//Crea una textura, la configura, la sube a OpenGL, 
//y devuelve el identificador de la textura 
//!!Por implementar
unsigned int loadTex(const char *fileName);

//Metodos para cargar obkjeto importado
void getVertexPos();
void getVertexIndex();
void getVertexColor();
void getVertexNormals();
void getVertexTextureCoords();
void getVertexTangents();
void cargarmodelonuevo();



int main(int argc, char** argv)
{
	scene = importer.ReadFile("Raptor.obj", aiProcessPreset_TargetRealtime_Fast);
	cargarmodelonuevo();

	std::locale::global(std::locale("spanish"));// acentos ;)

	initContext(argc, argv);
	initOGL();

	GLint num = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num);
	std::cout << num << std::endl;
	while (0<--num) {
		std::cout << glGetStringi(GL_EXTENSIONS, num - 1) << std::endl;
	}
	initShader("../shaders_P3/shader.v1.vert", "../shaders_P3/shader.v1.frag");
	initShader2("../shaders_P3/shader.v0.vert", "../shaders_P3/shader.v0.frag");
	initObj();
	
	
	//initObj();

	glutMainLoop();//Recibimos los eventos y los gestionamos

	destroy();

	return 0;
}

//////////////////////////////////////////
// Funciones auxiliares 
void initContext(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE); //Que no tenga retrocompatibilidad
											   //glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);//Inicializamos el frame buffer (,color, profundidad)
	glutInitWindowSize(500, 500);//tamaño de ventana 500x500
	glutInitWindowPosition(0, 0);//Empieza la ventana en el 0,0 ()
	glutCreateWindow("Prácticas GLSL");//Crea la ventana

	glewExperimental = GL_TRUE;//Inicializamos glew
	GLenum err = glewInit();//vemos si hay error
	if (GLEW_OK != err) {//Si lo hay
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;//Imprimimos el error
		exit(-1);
	}
	const GLubyte *oglVersion = glGetString(GL_VERSION);//Comprobamos que hemos inicializado bien glew
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;//Le pedimos la versión de opengl para comprobar que todo ok

																					//Le decimos a glut que funcion va a tratar que evento
	glutReshapeFunc(resizeFunc);
	glutDisplayFunc(renderFunc);
	glutIdleFunc(idleFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);

}
void initOGL() {
	glEnable(GL_DEPTH_TEST);//Hacer foto con el test de profundidad activado y desactivado 
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);//Color de fondo

	glFrontFace(GL_CCW);//Caras frente a nosotros son las de sentido antihorario (Ponerlo antihorario y horario)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//La cara de delante y de detras la rasterizamos
	glEnable(GL_CULL_FACE);//Activamos el culling para las caras de atras

	proj = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 300.0f);//Definimos la matriz de proyección (Apertura, Aspect ratio, near y far)
	view = glm::mat4(1.0f);//Definimos la matriz de vista
	view[3].z = -6;//La colocamos en z -6

}
void destroy() {
	glDetachShader(program, vshader);
	glDetachShader(program, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glDeleteProgram(program);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	if (inPos != -1) glDeleteBuffers(1, &posVBO);
	if (inColor != -1) glDeleteBuffers(1, &colorVBO);
	if (inNormal != -1) glDeleteBuffers(1, &normalVBO);
	if (inTexCoord != -1) glDeleteBuffers(1, &texCoordVBO);
	if (inTangent != -1) glDeleteBuffers(1, &tangentVBO);
	glDeleteBuffers(1, &triangleIndexVBO);


	glBindVertexArray(1);
	glDeleteVertexArrays(1, &vao); 
	glDeleteVertexArrays(1, &vao2);

	glDeleteTextures(1, &colorTexId);
	glDeleteTextures(1, &emiTexId);
	glDeleteTextures(1, &specTexId);
}
void initShader(const char *vname, const char *fname) {
	vshader = loadShader(vname, GL_VERTEX_SHADER);//Cargamos el shader de vertices con la ruta que nos pasan por parametro
	fshader = loadShader(fname, GL_FRAGMENT_SHADER);//Cargamos el shader de fragmentos con la ruta que nos pasan por parametro
	
	program = glCreateProgram();//Creamos un programa y nos devuelve el id de este
	glAttachShader(program, vshader);//Le añadimos el shader de vertices
	glAttachShader(program, fshader);//Le añadimos el shader de fragmentos

	glBindAttribLocation(program, 0, "inPos");//Especificamos que valor quiere que tenga la Pos
	glBindAttribLocation(program, 1, "inColor");//Especificamos que valor quiere que tenga el color
	glBindAttribLocation(program, 2, "inNormal");//Especificamos que valor quiere que tenga la la normal
	glBindAttribLocation(program, 3, "inTexCoord");////Especificamos que valor quiere que tenga la textura
	glBindAttribLocation(program, 4, "inTangent");////Especificamos que valor quiere que tenga la textura

	glLinkProgram(program);//Linkamos el programa

	int linked;//Estado del linkado
	glGetProgramiv(program, GL_LINK_STATUS, &linked);//Preguntamos por el estado
	if (!linked) {//Si ha fallado
				  //Calculamos una cadena de error 
		GLint logLen;//Longitud del error
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);//Preguntamos por la longitud
		char *logString = new char[logLen];//Creamos la cadena del error
		glGetProgramInfoLog(program, logLen, NULL, logString);//Guardamos el error en la cadena
		std::cout << "Error: " << logString << std::endl;//Imprimimos la cadena
		delete logString;//Borramos el array
		glDeleteProgram(program);//Borramos el programa
		program = 0;//Id = 0
		exit(-1);//Salimos con problema de compilación (-1)
	}

	uNormalMat = glGetUniformLocation(program, "normal");//Pedimos el identificador de la variable uniform normal
	uModelViewMat = glGetUniformLocation(program, "modelView");//Pedimos el identificador de la variable uniform modelView
	uModelViewProjMat = glGetUniformLocation(program, "modelViewProj");//Pedimos el identificador de la variable uniform modelViewProj
	uMyLightPos = glGetUniformLocation(program, "myLightPos");
	uIncLightIntensity = glGetUniformLocation(program, "incLightIntensity");
	uView = glGetUniformLocation(program, "View");

	uColorTex = glGetUniformLocation(program, "colorTex");//Pedimos el identificador ddde la variable uniforme de color de textura
	uEmiTex = glGetUniformLocation(program, "emiTex");//Pedimos el identificador ddde la variable uniforme de emi de textura
	uSpecTex = glGetUniformLocation(program, "specTex");//Pedimos el identificador ddde la variable uniforme de emi de textura
	uNormalTex = glGetUniformLocation(program, "normalTex");

	inPos = glGetAttribLocation(program, "inPos");//Pedimos el identificador del atributo inPos del shader
	inColor = glGetAttribLocation(program, "inColor");//Pedimos el identificador del atributo inColor del shader
	inNormal = glGetAttribLocation(program, "inNormal");//Pedimos el identificador del atributo inNormal del shader
	inTexCoord = glGetAttribLocation(program, "inTexCoord");//Pedimos el identificador del atributo inTextCoord del shader
	inTangent = glGetAttribLocation(program, "inTangent");//Pedimos el identificador del atributo inTextCoord del shader
}

void initShader2(const char *vname, const char *fname) {

	vshader2 = loadShader(vname, GL_VERTEX_SHADER);
	fshader2 = loadShader(fname, GL_FRAGMENT_SHADER);

	program2 = glCreateProgram();
	glAttachShader(program2, vshader2);
	glAttachShader(program2, fshader2);


	glBindAttribLocation(program2, 0, "inPos");
	glBindAttribLocation(program2, 1, "inColor");
	glBindAttribLocation(program2, 2, "inNormal");
	glBindAttribLocation(program2, 3, "inTexCoord");




	glLinkProgram(program2);




	GLint linked;
	glGetProgramiv(program2, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(program2, GL_INFO_LOG_LENGTH, &logLen);
		char *logString = new char[logLen];
		glGetProgramInfoLog(program2, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete logString;
		glDeleteProgram(program2);
		program2 = 0;
		exit(-1);
	}


	uNormalMat2 = glGetUniformLocation(program2, "normal");
	uModelViewMat2 = glGetUniformLocation(program2, "modelView");
	uModelViewProjMat2 = glGetUniformLocation(program2, "modelViewProj");

	uColorTex2 = glGetUniformLocation(program2, "colorTex");
	uEmiTex2 = glGetUniformLocation(program2, "emiTex");


	inPos2 = glGetAttribLocation(program2, "inPos");
	inColor2 = glGetAttribLocation(program2, "inColor");
	inNormal2 = glGetAttribLocation(program2, "inNormal");
	inTexCoord2 = glGetAttribLocation(program2, "inTexCoord");





}

void initObj() {
	glGenVertexArrays(1, &vao);//Creamos 1 VAo y nos devuelve el identificador
	glBindVertexArray(vao);//Activamos el VAO para configurarlo
	//Calculamos tangentes y normales
	sacarpuntos();
	if (inPos != -1) {//Si existe ese atributo
		glGenBuffers(1, &posVBO);//Generamos un buffer para la posición de los vertices
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);//Activamos el buffer para poder modificarlo
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3, cubeVertexPos, GL_STATIC_DRAW);//(Le decimos que número vamos a subir los datos, de donde y el tipo de buffer)
		glVertexAttribPointer(inPos, 3, GL_FLOAT, GL_FALSE, 0, 0);//Configuramos el buffer (que atributo, num coordenadas, tipo, si lo normalizamos, si tenemos strite o offset)
		glEnableVertexAttribArray(inPos);//Habilita el atributo
	}
	if (inColor != -1) {//Lo mismo para el color
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3, cubeVertexColor, GL_STATIC_DRAW);
		glVertexAttribPointer(inColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inColor);
	}
	if (inNormal != -1) {//Lo mismo para las normales
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3, lolnormal, GL_STATIC_DRAW);
		glVertexAttribPointer(inNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inNormal);
	}
	if (inTexCoord != -1) {//Lo mismo para las texturas
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 2, cubeVertexTexCoord, GL_STATIC_DRAW);
		glVertexAttribPointer(inTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTexCoord);
	}
	if (inTangent != -1) {
		glGenBuffers(1, &tangentVBO);//Generamos un buffer para la posición de los vertices
		glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);//Activamos el buffer para poder modificarlo
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3, cubeVertexTangent, GL_STATIC_DRAW);//(Le decimos que número vamos a subir los datos, de donde y el tipo de buffer)
		glVertexAttribPointer(inTangent, 3, GL_FLOAT, GL_FALSE, 0, 0);//Configuramos el buffer (que atributo, num coordenadas, tipo, si lo normalizamos, si tenemos strite o offset)
		glEnableVertexAttribArray(inTangent);//Habilita el atributo
	}

	glGenBuffers(1, &triangleIndexVBO);//Generamos un vbo para los indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);//Activamos el VBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeNTriangleIndex * sizeof(unsigned int) * 3, cubeTriangleIndex, GL_STATIC_DRAW);//Subimos los datos

	model = glm::mat4(1.0f);

	colorTexId = loadTex("../img/color2.png");//Cargamos la textura de color
	emiTexId = loadTex("../img/emissive.png");//Cargamos la textura emisiva
	specTexId = loadTex("../img/specMap.png");//Cargamos la textura especular
	normalTexId = loadTex("../img/normal.png");
	tex2 = loadTex("../img/normal.png");
	tex3 = loadTex("../img/normal.png");

	glGenVertexArrays(2, &vao2);//Creamos 1 VAo y nos devuelve el identificador
	glBindVertexArray(vao2);//Activamos el VAO para configurarlo

	
	if (inPos != -1) {//Si existe ese atributo
		glGenBuffers(1, &posVBO);//Generamos un buffer para la posición de los vertices
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);//Activamos el buffer para poder modificarlo
		glBufferData(GL_ARRAY_BUFFER, miNumVertex * sizeof(float) * 3, miVertexPos, GL_STATIC_DRAW);//(Le decimos que número vamos a subir los datos, de donde y el tipo de buffer)
		glVertexAttribPointer(inPos, 3, GL_FLOAT, GL_FALSE, 0, 0);//Configuramos el buffer (que atributo, num coordenadas, tipo, si lo normalizamos, si tenemos strite o offset)
		glEnableVertexAttribArray(inPos);//Habilita el atributo
	}
	if (inColor != -1) {//Lo mismo para el color
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, miNumVertex * sizeof(float) * 3, miModelColor, GL_STATIC_DRAW);
		glVertexAttribPointer(inColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inColor);
	}
	/*for (int i = 0; i < miNumVertex * 3; i++) {
		std::cout << miVertexTang[i] << " " << tangent[i] << std::endl;
	}*/
	if (inNormal != -1) {//Lo mismo para las normales
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, miNumVertex * sizeof(float) * 3, miNormals, GL_STATIC_DRAW);
		glVertexAttribPointer(inNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inNormal);
	}
	if (inTexCoord != -1) {//Lo mismo para las texturas
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, miNumVertex * sizeof(float) * 2, miTexCoord, GL_STATIC_DRAW);
		glVertexAttribPointer(inTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTexCoord);
	}
	glGenBuffers(2, &triangleIndexVBO);//Generamos un vbo para los indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);//Activamos el VBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, miNumFaces * sizeof(unsigned int) * 3, miVertexIndex, GL_STATIC_DRAW);//Subimos los datos

	
	verde = loadTex("../img/dino.jpg");


	//Objeto3
	glGenVertexArrays(3, &vao3);
	glBindVertexArray(vao3);

	if (inPos2 != -1)
	{
		glGenBuffers(1, &posVBO);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
			cubeVertexPos, GL_STATIC_DRAW);
		glVertexAttribPointer(inPos2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inPos2);
	}
	if (inColor2 != -1)
	{
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
			cubeVertexColor, GL_STATIC_DRAW);
		glVertexAttribPointer(inColor2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inColor2);
	}
	if (inNormal2 != -1)
	{
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
			cubeVertexNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(inNormal2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inNormal2);
	}
	if (inTexCoord2 != -1)
	{
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 2,
			cubeVertexTexCoord, GL_STATIC_DRAW);
		glVertexAttribPointer(inTexCoord2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTexCoord2);
	}

	glGenBuffers(1, &triangleIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		cubeNTriangleIndex * sizeof(unsigned int) * 3, cubeTriangleIndex,
		GL_STATIC_DRAW);

	model3 = glm::mat4(1.0f);

	colorTexId2 = loadTex("../img/color2.png");
	emiTexId2 = loadTex("../img/emissive.png");
}


GLuint loadShader(const char *fileName, GLenum type) {
	unsigned int fileLen;//Longitud del fichero
	char *source = loadStringFromFile(fileName, fileLen);// Carga el fichero y nos devuelve la longitud del fichero y una cadena de caracteres con el fichero (Nombre del shader, tipo del shader de vertices o fragmentos)

														 //Creación y compilación del Shader 
	GLuint shader;//Crea shader Id
	shader = glCreateShader(type);//Creamos el tipo de shader y nos devuelve un identificador para ese shader
	glShaderSource(shader, 1, (const GLchar **)&source, (const GLint *)&fileLen);//Le decimos a opengl que el codigo de shader esta en la lista de caracteres y su longitud
	glCompileShader(shader);//Compilamos el shader
	delete source;//Borramos el puntero

				  //Comprobamos que se compiló bien
	GLint compiled;//Variable para comprobar el estado de la compilación
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);//Comprobamos el estado de la compilacion preguntandoselo a opengl
	if (!compiled) {
		//Calculamos una cadena de error 
		GLint logLen;//Declaramos la longitud de la cadena de eroor
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);//Le preguntamos la longitud
		char *logString = new char[logLen];//Array donde vamos a meter la cadena
		glGetShaderInfoLog(shader, logLen, NULL, logString);//Guardamos el error en la cadena
		std::cout << "Error: " << logString << std::endl;//Imprimimos el error
		delete logString;//Borramos la memoria
		glDeleteShader(shader);//Borramos el shader
		exit(-1);//Salimos del programa con -1, (error de compilación)
	}

	return shader;//Devolvemos el identificador del shader
}
unsigned int loadTex(const char *fileName) {
	unsigned char *map;
	unsigned int w, h;
	map = loadTexture(fileName, w, h);//Le das el nombre y te devuelve el alto y ancho de la imagen y te devuelve la información del color en un array
	if (!map) {//Comprobamos si ha cargado bien el fichero 
			   //Si no carga mostramos un error
		std::cout << "Error cargando el fichero: " << fileName << std::endl;
		exit(-1);
	}

	unsigned int texId;
	glGenTextures(1, &texId);//Generar textura en opengl
	glBindTexture(GL_TEXTURE_2D, texId);//Activamos la textura que queremos modificar como una textura 2d
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)map);//Subimos la textura a la memoria de la tarjeta grafica

	delete[] map;//Libera la memoria de cpu

	glGenerateMipmap(GL_TEXTURE_2D);//Genera el mipmap de la textura activada

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//Cuando dos fragmentos consecutivos se corresponden con dos puntos de textura muy separadas, vamos a una imagen mas pequeña para poder asociar mejor los puntos a los fragmentos
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//Se interpolan linealmente los texel
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);//Las coordenadas de textura por encima de 1 las trunca a 1
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);//Las coordenadas de textura por debajo de 0 las trunca a 0

	//Filtro Anisotrópico
	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	return texId;//Devolver el identificador al que nos haya pedido la textura
}

void renderFunc() {
	glm::mat4 modelAssimp = glm::mat4(1.0f);
	modelAssimp = glm::translate(modelAssimp, glm::vec3(6.0, 0, 0));

	model2 = glm::translate(model, glm::vec3(2.5f));
	glm::mat4 model3 = glm::mat4(1.0f);
	glm::mat4 model4 = glm::mat4(1.0f);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Limpiamos el buffer de color y de profundidad del back

	glUseProgram(program); //Activamos el programa

						   // pintado del objeto!!!! 
						   //Matrices uniform las creamos en la memoria principal
	
	glm::mat4 modelView = view * model;
	glm::mat4 modelViewProj = proj * view * model;
	glm::mat4 normal = glm::transpose(glm::inverse(modelView));
	//Las subimos al shader
	if (uModelViewMat != -1) glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE, &(modelView[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	if (uModelViewProjMat != -1) glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE, &(modelViewProj[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	if (uNormalMat != -1) glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, &(normal[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	if (uView != -1) glUniformMatrix4fv(uView, 1, GL_FALSE, &(view[0][0]));
	if (uIncLightIntensity != -1) glUniform3fv(uIncLightIntensity, 1, &(lightintensity.x));
	if (uMyLightPos != -1) glUniform3fv(uMyLightPos, 1, &(lightpos.x));

	//Texturas
	if (uColorTex != -1) {
		glActiveTexture(GL_TEXTURE0);//Activamos la textura
		glBindTexture(GL_TEXTURE_2D, colorTexId);//le asociamos la textura
		glUniform1i(uColorTex, 0);//Le decimos que variable uniforme la va a usar
	}
	if (uEmiTex != -1) {
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, emiTexId);
		glUniform1i(uEmiTex, 1);
	}
	if (uSpecTex != -1) {
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, specTexId);
		glUniform1i(uSpecTex, 2);
	}
	if (normalTexId != -1) {
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, normalTexId);
		glUniform1i(uNormalTex, 3);
	}

	glBindVertexArray(vao);//Activar el VAO de la configuración de los atributos
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, (void*)0);//Pinte los triangulos que estan su direccion(indices) en el VAO (triangulos, tamaño, tipo, desde el principio del array)

  

	////Objeto Plano
	//model4 = glm::scale(model4, glm::vec3(50,0,0));
	//modelView = view * model4;
	//modelViewProj = proj * view * model4;
	//normal = glm::transpose(glm::inverse(modelView));
	////Las subimos al shader
	//if (uModelViewMat != -1) glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE, &(modelView[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	//if (uModelViewProjMat != -1) glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE, &(modelViewProj[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	//if (uNormalMat != -1) glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, &(normal[0][0]));

	//if (uColorTex != -1) {
	//	glActiveTexture(GL_TEXTURE0 + 2);//Activamos la textura
	//	glBindTexture(GL_TEXTURE_2D, tex2);//le asociamos la textura
	//	glUniform1i(uColorTex, 2);
	//}

	//glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3, GL_UNSIGNED_INT, (void*)0);

	// Objeto 3 (Curva de Bezier grado 3)
	glm::mat3 pcontrol = glm::transpose(glm::mat3(glm::vec3(-4.0, 0.0, 0.0), glm::vec3(0.0, 4.0, 0.0), glm::vec3(4.0, 0.0, 0.0)));//Matriz de puntos de control
	glm::mat3 curva = glm::transpose(glm::mat3(glm::vec3(1.0, 0.0, 0.0), glm::vec3(-2.0, 2.0, 0.0), glm::vec3(1.0, -2.0, 1.0)));//Matriz de la curva
	glm::vec3 tiempo = glm::vec3(1.0, t, t*t);//Matriz del tiempo
	glm::vec3 resultado = tiempo * curva * pcontrol; //puntos x , y ,z en cada momento 

													 //std::cout << resultado[0] << " " << resultado[1] << " " << resultado[2] << std::endl;

	model3 = glm::translate(model3, glm::vec3(model[3].x, model[3].y, model[3].z));
	model3 = glm::translate(model3, resultado);
	//std::cout << model3[3].x << " " << model3[3] .y << " " << model3[3].z << std::endl;

	t = t + c;//Variamos el tiempo de 0->1 y de 1->0
	if (t >= 1)
		c = -0.0001;
	if (t <= 0)
		c = 0.0001;

	modelView = view * model3;
	modelViewProj = proj * view * model3;
	normal = glm::transpose(glm::inverse(modelView));
	if (uModelViewMat != -1) glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE, &(modelView[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	if (uModelViewProjMat != -1) glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE, &(modelViewProj[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	if (uNormalMat != -1) glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, &(normal[0][0]));

	if (uColorTex != -1) {
		glActiveTexture(GL_TEXTURE0 + 0);//Activamos la textura
		glBindTexture(GL_TEXTURE_2D, tex3);//le asociamos la textura
		glUniform1i(uColorTex, 0);
	}
	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3, GL_UNSIGNED_INT, (void*)0);

	//OBJETO 4
	modelAssimp = glm::scale(modelAssimp, glm::vec3(0.05, 0.05, 0.05));
	modelView = view * modelAssimp;
	modelViewProj = proj * view * modelAssimp;
	normal = glm::transpose(glm::inverse(modelView));
	//Las subimos al shader
	if (uModelViewMat != -1) glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE, &(modelView[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	if (uModelViewProjMat != -1) glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE, &(modelViewProj[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	if (uNormalMat != -1) glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, &(normal[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	if (uView != -1) glUniformMatrix4fv(uView, 1, GL_FALSE, &(view[0][0]));

	//Texturas
	if (uColorTex != -1) {
		glActiveTexture(GL_TEXTURE0 + 4);//Activamos la textura
		glBindTexture(GL_TEXTURE_2D, verde);//le asociamos la textura
		glUniform1i(verde, 4);//Le decimos que variable uniforme la va a usar
	}

	glBindVertexArray(vao2);//Activar el VAO de la configuración de los atributos

	glDrawElements(GL_TRIANGLES, miNumFaces * 3, GL_UNSIGNED_INT, (void*)0);

	//Objeto 2
	glUseProgram(program2);
	modelView = view * model2;
	modelViewProj = proj * view * model2;
	normal = glm::transpose(glm::inverse(modelView));
	//Las subimos al shader
	if (uModelViewMat2 != -1) glUniformMatrix4fv(uModelViewMat2, 1, GL_FALSE, &(modelView[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	if (uModelViewProjMat2 != -1) glUniformMatrix4fv(uModelViewProjMat2, 1, GL_FALSE, &(modelViewProj[0][0]));//(Matriz,id,no trasponer, dirección del primer elemento)
	if (uNormalMat2 != -1) glUniformMatrix4fv(uNormalMat2, 1, GL_FALSE, &(normal[0][0]));

	if (uColorTex2 != -1) {
		glActiveTexture(GL_TEXTURE0 + 5);//Activamos la textura
		glBindTexture(GL_TEXTURE_2D, tex2);//le asociamos la textura
		glUniform1i(uColorTex, 5);
	}
	glBindVertexArray(vao3);//Activar el VAO de la configuración de los atributos
	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3, GL_UNSIGNED_INT, (void*)0);


//Se alternan el front y el back de los buffers, front(se muestra) back (pintamos)

	glUseProgram(NULL);//Desabilitamos el programa


	glutSwapBuffers();
}


void resizeFunc(int width, int height) {
	glViewport(0, 0, width, height);
	float aspectratio = (float(width) / float(height));

	proj[0].x = 1.0 / (aspectratio*(tan(M_PI / 6)));
	proj[1].y = 1.0 / (tan(M_PI / 6));
	glutPostRedisplay();

}

void idleFunc() {
	//Modificamos la matriz model
	
	model = glm::mat4(1.0f);
	static float angle = 0.0f;
	angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.0001f;
	model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 0.0f));


	
	glutPostRedisplay();//LLamamos a la funcion render a través de esta función
}
void keyboardFunc(unsigned char key, int x, int y) {
	glm::mat4 viewAux = glm::mat4(1.0f);

	std::cout << "Se ha pulsado la tecla " << key << std::endl << std::endl;
	switch (key) {
	case 'w':
		viewAux[3].z = viewAux[3].z + 2.0f;
		break;
	case 'a':
		viewAux[3].x = viewAux[3].x + 2.0f;
		break;
	case 's':
		viewAux[3].z = viewAux[3].z - 2.0f;
		break;
	case 'd':
		viewAux[3].x = viewAux[3].x - 2.0f;
		break;
	case 'q':
		viewAux = glm::rotate(viewAux, 0.1f, glm::vec3(0.0, -1.0, 0.0));
		break;
	case 'e':
		viewAux = glm::rotate(viewAux, 0.1f, glm::vec3(0.0, 1.0, 0.0));
		break;
	case 'j':
		//Mueve la luz hacia la izquierda
		lightpos = lightpos + glm::vec3(-1.0, 0, 0);
		break;
	case 'k':
		//Mueve la luz hacia abajo
		lightpos = lightpos + glm::vec3(0, -1.0, 0);
		break;
	case 'l':
		//Mueve la luz hacia la derecha
		lightpos = lightpos + glm::vec3(1.0, 0, 0);
		break;
	case 'i':
		//Mueve la luz hacia arriba
		lightpos = lightpos + glm::vec3(0, 1.0, 0);
		break;
	case 'u':
		//Mueve la luz hacia atras
		lightpos = lightpos + glm::vec3(0, 0, 1.0);
		break;
	case 'o':
		//Mueve la luz hacia delante
		lightpos = lightpos + glm::vec3(0, 0, -1.0);
		break;
	case 'y':
		if (lightintensity.x < 1)lightintensity += glm::vec3(0.1, 0.1, 0.1);

		break;
	case 'h':
		if (lightintensity.x > 0)lightintensity -= glm::vec3(0.1, 0.1, 0.1);
		break;
	case 27:
		exit(0);
		break;
	}

	view = viewAux * view;
	//lightpos = glm::vec3(view*glm::vec4(lightpos, 1.0f));
	glutPostRedisplay();
}

void mouseFunc(int button, int state, int x, int y) {
	glm::mat4 rotation = glm::mat4(1.0);

	if (state == 0)
		std::cout << "Se ha pulsado el botón ";
	else
		std::cout << "Se ha soltado el botón ";

	if (button == 0) {
		std::cout << "de la izquierda del ratón " << std::endl;
		rotation = glm::rotate(rotation, 0.1f, glm::vec3(0.0, -1.0, 0.0));
		view = rotation * view;
	}

	//if (button == 1)
	//std::cout << "central del ratón " << std::endl;

	if (button == 2) std::cout << "de la derecha del ratón " << std::endl; {
		rotation = glm::rotate(rotation, 0.1f, glm::vec3(0.0, 1.0, 0.0));
		view = rotation * view;
	}
	glutPostRedisplay();
	std::cout << "en la posición " << x << " " << y << std::endl << std::endl;
}

//
//Tangentes y Normales
//
void sacarpuntos() {
	int k = 0;
	for (int i = 0; i < 36; i += 3) {
		glm::vec3 P0 = glm::vec3(cubeVertexPos[cubeTriangleIndex[i] * 3], cubeVertexPos[cubeTriangleIndex[i] * 3 + 1], cubeVertexPos[cubeTriangleIndex[i] * 3 + 2]);
		glm::vec3 P1 = glm::vec3(cubeVertexPos[cubeTriangleIndex[i + 1] * 3], cubeVertexPos[cubeTriangleIndex[i + 1] * 3 + 1], cubeVertexPos[cubeTriangleIndex[i + 1] * 3 + 2]);
		glm::vec3 P2 = glm::vec3(cubeVertexPos[cubeTriangleIndex[i + 2] * 3], cubeVertexPos[cubeTriangleIndex[i + 2] * 3 + 1], cubeVertexPos[cubeTriangleIndex[i + 2] * 3 + 2]);

		glm::vec2 uv0 = glm::vec2(cubeVertexTexCoord[cubeTriangleIndex[i] * 2], cubeVertexTexCoord[cubeTriangleIndex[i] * 2 + 1]);
		glm::vec2 uv1 = glm::vec2(cubeVertexTexCoord[cubeTriangleIndex[i + 1] * 2], cubeVertexTexCoord[cubeTriangleIndex[i + 1] * 2 + 1]);
		glm::vec2 uv2 = glm::vec2(cubeVertexTexCoord[cubeTriangleIndex[i + 2] * 2], cubeVertexTexCoord[cubeTriangleIndex[i + 2] * 2 + 1]);

		/*
		std::cout << " P0 " << P0.x << " " << P0.y << " " << P0.z << std::endl;
		std::cout << " P1 " << P1.x << " " << P1.y << " " << P1.z << std::endl;
		std::cout << " P2 " << P2.x << " " << P2.y << " " << P2.z << std::endl;
		std::cout << "\n";*/

		glm::vec3 V0 = glm::vec3(P1.x - P0.x, P1.y - P0.y, P1.z - P0.z);
		glm::vec3 V1 = glm::vec3(P2.x - P0.x, P2.y - P0.y, P2.z - P0.z);
		glm::vec2 st1 = glm::vec2(uv1.x - uv0.x, uv1.y - uv0.y);
		glm::vec2 st2 = glm::vec2(uv2.x - uv0.x, uv2.y - uv0.y);

		generatetangent(V0, V1, st1, st2, i);
		if (i % 2 == 0) {
			generatenormal(V0, V1, i, k);
			k = k + 12;
		}
	}
}

void sacarpuntos(float posvertices[], unsigned int indvertices[], float texcoord[], int numcaras) {
	int k = 0;
	for (int i = 0; i < numcaras; i += 3) {
		glm::vec3 P0 = glm::vec3(posvertices[indvertices[i] * 3], posvertices[indvertices[i] * 3 + 1], posvertices[indvertices[i] * 3 + 2]);
		glm::vec3 P1 = glm::vec3(posvertices[indvertices[i + 1] * 3], posvertices[indvertices[i + 1] * 3 + 1], posvertices[indvertices[i + 1] * 3 + 2]);
		glm::vec3 P2 = glm::vec3(posvertices[indvertices[i + 2] * 3], posvertices[indvertices[i + 2] * 3 + 1], posvertices[indvertices[i + 2] * 3 + 2]);

		glm::vec2 uv0 = glm::vec2(texcoord[indvertices[i] * 2], texcoord[indvertices[i] * 2 + 1]);
		glm::vec2 uv1 = glm::vec2(texcoord[indvertices[i + 1] * 2], texcoord[indvertices[i + 1] * 2 + 1]);
		glm::vec2 uv2 = glm::vec2(texcoord[indvertices[i + 2] * 2], texcoord[indvertices[i + 2] * 2 + 1]);

		/*
		std::cout << " P0 " << P0.x << " " << P0.y << " " << P0.z << std::endl;
		std::cout << " P1 " << P1.x << " " << P1.y << " " << P1.z << std::endl;
		std::cout << " P2 " << P2.x << " " << P2.y << " " << P2.z << std::endl;
		std::cout << "\n";*/

		glm::vec3 V0 = glm::vec3(P1.x - P0.x, P1.y - P0.y, P1.z - P0.z);
		glm::vec3 V1 = glm::vec3(P2.x - P0.x, P2.y - P0.y, P2.z - P0.z);
		glm::vec2 st1 = glm::vec2(uv1.x - uv0.x, uv1.y - uv0.y);
		glm::vec2 st2 = glm::vec2(uv2.x - uv0.x, uv2.y - uv0.y);

		generatetangent(V0, V1, st1, st2, i,indvertices);
		if (i % 2 == 0) {
			generatenormal(V0, V1, i, k);
			k = k + 12;
		}
	}
}
//
//Tangente
//
void generatetangent(glm::vec3 v0, glm::vec3 v1, glm::vec2 st1, glm::vec2 st2, int i) {
	float coef = 1 / (st1.x*st2.y - st1.y * st2.x);

	tangent[cubeTriangleIndex[i] * 3] += coef*(v0.x*st2.y - v1.x*st1.y);//Coordenada x del primer vertice del triangulo i/3
	tangent[cubeTriangleIndex[i] * 3 + 1] += coef*(v0.y*st2.y - v1.y*st1.y);//Coordenada y del primer vertice del triangulo i/3
	tangent[cubeTriangleIndex[i] * 3 + 2] += coef*(v0.z*st2.y - v1.z*st1.y);//Coordenada z del primer vertice del triangulo i/3

	tangent[cubeTriangleIndex[i + 1] * 3] += coef*(v0.x*st2.y - v1.x*st1.y);//Coordenada x del segundo vertice del triangulo i/3
	tangent[cubeTriangleIndex[i + 1] * 3 + 1] += coef*(v0.y*st2.y - v1.y*st1.y);//Coordenada y del segundo vertice del triangulo i/3
	tangent[cubeTriangleIndex[i + 1] * 3 + 2] += coef*(v0.z*st2.y - v1.z*st1.y);//Coordenada z del segundo vertice del triangulo i/3

	tangent[cubeTriangleIndex[i + 2] * 3] += coef*(v0.x*st2.y - v1.x*st1.y);//Coordenada x del tercer vertice del triangulo i/3
	tangent[cubeTriangleIndex[i + 2] * 3 + 1] += coef*(v0.y*st2.y - v1.y*st1.y);//Coordenada y del tercer vertice del triangulo i/3
	tangent[cubeTriangleIndex[i + 2] * 3 + 2] += coef*(v0.z*st2.y - v1.z*st1.y);//Coordenada z del tercer vertice del triangulo i/3

}
void generatetangent(glm::vec3 v0, glm::vec3 v1, glm::vec2 st1, glm::vec2 st2, int i,unsigned int indvertices[]) {
	float coef = 1 / (st1.x*st2.y - st1.y * st2.x);

	tangent[indvertices[i] * 3] += coef*(v0.x*st2.y - v1.x*st1.y);//Coordenada x del primer vertice del triangulo i/3
	tangent[indvertices[i] * 3 + 1] += coef*(v0.y*st2.y - v1.y*st1.y);//Coordenada y del primer vertice del triangulo i/3
	tangent[indvertices[i] * 3 + 2] += coef*(v0.z*st2.y - v1.z*st1.y);//Coordenada z del primer vertice del triangulo i/3

	tangent[indvertices[i + 1] * 3] += coef*(v0.x*st2.y - v1.x*st1.y);//Coordenada x del segundo vertice del triangulo i/3
	tangent[indvertices[i + 1] * 3 + 1] += coef*(v0.y*st2.y - v1.y*st1.y);//Coordenada y del segundo vertice del triangulo i/3
	tangent[indvertices[i + 1] * 3 + 2] += coef*(v0.z*st2.y - v1.z*st1.y);//Coordenada z del segundo vertice del triangulo i/3

	tangent[indvertices[i + 2] * 3] += coef*(v0.x*st2.y - v1.x*st1.y);//Coordenada x del tercer vertice del triangulo i/3
	tangent[indvertices[i + 2] * 3 + 1] += coef*(v0.y*st2.y - v1.y*st1.y);//Coordenada y del tercer vertice del triangulo i/3
	tangent[indvertices[i + 2] * 3 + 2] += coef*(v0.z*st2.y - v1.z*st1.y);//Coordenada z del tercer vertice del triangulo i/3

}

//
//Normal
//
void generatenormal(glm::vec3 v0, glm::vec3 v1, int i, int k) {
	glm::vec3 aux;

	aux = glm::normalize(glm::cross(v0, v1));
	for (int j = i; j < i + 4; j++) {

		lolnormal[k] = aux.x;
		lolnormal[k + 1] = aux.y;
		lolnormal[k + 2] = aux.z;
		//std::cout << " P " << j << " " << lolnormal[k] << " " << lolnormal[k + 1] << " " << lolnormal[k + 2] << std::endl;

		k = k + 3;
	}
}


void getVertexPos() {
	int aux = 0;
	for (int i = 0; i < miNumVertex; i++) {
		for (int j = 0; j < 3; j++) {
			miVertexPos [aux] = scene->mMeshes[0]->mVertices[i][j];
			aux++;
		}

	}
}
void getVertexIndex() {
	int aux = 0;
	for (int i = 0; i < miNumFaces; i++) {
		for (int j = 0; j < 3; j++) {
			miVertexIndex[aux] = scene->mMeshes[0]->mFaces[i].mIndices[j];
			aux++;
		}

	}
}
void getVertexColor() {
	int aux = 0;
	for (int i = 0; i < miNumVertex*3; i++) {
		miModelColor[i] = 1;

	}
}
void getVertexNormals() {
	int aux = 0;
	for (int i = 0; i < miNumVertex; i++) {
		for (int j = 0; j < 3; j++) {
			miNormals[aux] = scene->mMeshes[0]->mNormals[i][j];
			aux++;
		}

	}
}
void getVertexTextureCoords() {
	int aux = 0;
	for (int i = 0; i < miNumVertex; i++) {
		for (int j = 0; j < 2; j++) {
			miTexCoord[aux] = scene->mMeshes[0]->mTextureCoords[0][i][j];
			aux++;
		}

	}
}
void getVertexTangents() {
	int aux = 0;
	for (int i = 0; i < miNumVertex; i++) {
		for (int j = 0; j < 3; j++) {
			miVertexTang[aux] = scene->mMeshes[0]->mTangents[i][j];
			aux++;
		}

	}


}
void cargarmodelonuevo() {
	miNumVertex = scene->mMeshes[0]->mNumVertices;
	miNumFaces = scene->mMeshes[0]->mNumFaces;

	getVertexIndex();
	getVertexPos();
	getVertexColor();
	getVertexNormals();
	getVertexTextureCoords();
	getVertexTangents();


}



