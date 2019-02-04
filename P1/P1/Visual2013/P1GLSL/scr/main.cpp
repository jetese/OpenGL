#include "BOX.h"
#include <IGL/IGlib.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#define M_PI 3.1415


//Idenficadores de los objetos de la escena
int objId = -1;
int obj2Id = -2;
int obj3Id = -3;
int obj4Id = -4;
int prex = 0; //x antes del movimiento
float t = 0;//tiempo para la curva de bezier
float c = 0.0001;
			  //Declaración de CB
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);

//Declaración de las matrices proj y view como globales
glm::mat4 proj = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));
	if (!IGlib::init("../shaders_P1/shader.v9.vert", "../shaders_P1/shader.v9.frag"))
		return -1;

	//CBs
	IGlib::setResizeCB(resizeFunc);
	IGlib::setIdleCB(idleFunc);
	IGlib::setKeyboardCB(keyboardFunc);
	IGlib::setMouseCB(mouseFunc);
	IGlib::setMouseMoveCB(mouseMotionFunc);

	//Parámetro matriz view
	view[3].z = -8;

	//Parámetros de la matriz de proyección
	float f = 1.0 / tan(M_PI / 6);
	float near = 1;
	float far = 100;
	proj[0].x = f;
	proj[1].y = f;
	proj[2].z = (far + near) / (near - far);
	proj[2].w = -1;
	proj[3].z = (2 * far*near) / (near - far);
	proj[3].w = 0;


	//Se ajusta la cámara
	IGlib::setProjMat(proj);
	IGlib::setViewMat(view);


	//Creamos los objetos a visualizar
	objId = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex,
		cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);
	obj2Id = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex,
		cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);
	obj3Id = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex,
		cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);
	obj4Id = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, pyramidTriangleIndex,
		pyramidVertexPos, pyramidVertexColor, pyramidVertexNormal, pyramidVertexTexCoord, pyramidVertexTangent);
	//Incluir texturas aquí.
	IGlib::addColorTex(objId, "../img/color.png");
	IGlib::addColorTex(obj2Id, "../img/color.png");
	IGlib::addColorTex(obj3Id, "../img/color.png");


	//Mainloop
	IGlib::mainLoop();
	IGlib::destroy();
	return 0;
}

void resizeFunc(int width, int height)
{
	float aspectratio = (float(width) / float(height));

	proj[0].x = 1.0 / (aspectratio*(tan(M_PI / 6)));
	proj[1].y = 1.0 / (tan(M_PI / 6));

	IGlib::setProjMat(proj);

}

void idleFunc()
{
	static float angle = 0.0f;
	angle = (angle > M_PI * 2.0f) ? 0 : angle + 0.001f;

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 model2 = glm::mat4(1.0f);
	glm::mat4 model3 = glm::mat4(1.0f);
	glm::mat4 model4 = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, angle, glm::vec3(1.0, 1.0, 0));
	
	IGlib::setModelMat(objId, model);
	//Objeto 3 (Cubo que orbita alrededor del primero describiendo una circunferencia)
	model2 = glm::translate(model2, glm::vec3 (model[3].x, model[3].y, model[3].z));
	model2 = glm::rotate(model2, angle, glm::vec3(0.0, 1.0, 0.0));
	model2 = glm::translate(model2, glm::vec3(4.0, 0.0, 0.0));

	IGlib::setModelMat(obj2Id, model2);


	//está editado. de la otra forma sería model2=identidad
	//Objeto 3 (Curva de Bezier grado 3)
	glm::mat3 pcontrol = glm::transpose(glm::mat3(glm::vec3(-8,0, 0), glm::vec3(0,8,0), glm::vec3(8,0,0)));//Matriz de puntos de control
	glm::mat3 curva  = glm::transpose(glm::mat3(glm::vec3(1, 0, 0), glm::vec3(-2, 2, 0), glm::vec3(1, -2, 1)));//Matriz de la curva
	glm::vec3 tiempo = glm::vec3(1, t, t*t);//Matriz del tiempo
	glm::vec3 resultado = tiempo * curva * pcontrol; //puntos x , y ,z en cada momento 

	//std::cout << resultado[0] << " " << resultado[1] << " " << resultado[2] << std::endl;

	model3 = glm::translate(model3, glm::vec3(model[3].x, model[3].y, model[3].z));
	model3 = glm::translate(model3, resultado);
	
	t = t + c;//Variamos el tiempo de 0->1 y de 1->0
	if (t >= 1)
		c = -0.0001;
	if (t <= 0)
		c = 0.0001;
	IGlib::setModelMat(obj3Id, model3);

	//Objeto 4 Piramide
	model4 = glm::translate(model4, glm::vec3(model[3].x, model[3].y, model[3].z));
	model4 = glm::rotate(model4, angle, glm::vec3(0.0, 1.0, 0.0));
	model4 = glm::translate(model4, glm::vec3(0.0,-4.0, 0.0));
	IGlib::setModelMat(obj4Id, model4);

}

void keyboardFunc(unsigned char key, int x, int y)
{
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
	case 27:
		exit(0);
		break;
	}

	view = viewAux * view;
	IGlib::setViewMat(view);
}

void mouseFunc(int button, int state, int x, int y)
{
	glm::mat4 rotation = glm::mat4(1.0);

	if (state == 0)
		std::cout << "Se ha pulsado el botón ";
	else
		std::cout << "Se ha soltado el botón ";

	if (button == 0) {
		std::cout << "de la izquierda del ratón " << std::endl;
		rotation = glm::rotate(rotation, 0.1f, glm::vec3(0.0, -1.0, 0.0));
		view = rotation * view;
		IGlib::setViewMat(view);
	}

	//if (button == 1)
	//std::cout << "central del ratón " << std::endl;

	if (button == 2) std::cout << "de la derecha del ratón " << std::endl; {
		rotation = glm::rotate(rotation, 0.1f, glm::vec3(0.0, 1.0, 0.0));
		view = rotation * view;
		IGlib::setViewMat(view);
	}

	std::cout << "en la posición " << x << " " << y << std::endl << std::endl;
}

void mouseMotionFunc(int x, int y)
{
	glm::mat4 rotation = glm::mat4(1.0);
	if (x < prex) {
		rotation = glm::rotate(rotation, 0.1f, glm::vec3(0.0, -1.0, 0.0));
	}
	else if (x > prex) {
		rotation = glm::rotate(rotation, 0.1f, glm::vec3(0.0, 1.0, 0.0));
	}
	prex = x;
	view = rotation * view;
	IGlib::setViewMat(view);
}