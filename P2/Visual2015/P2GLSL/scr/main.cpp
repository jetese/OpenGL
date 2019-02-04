#include "BOX.h"
#include <IGL/IGlib.h>
#define M_PI 3.1415
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>




//Idenficadores de los objetos de la escena
int objId = -1;

//PARA EL RATÓN
int prex = 0; //x antes del movimiento


			  //Declaración de CB
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);
void generatetangent(glm::vec3 v0, glm::vec3 v1, glm::vec2 st1, glm::vec2 st2, int i);
void sacarpuntos();
void generatenormal(glm::vec3 v0, glm::vec3 v1, int i,int k);

float tangent[72];
float lolnormal[72];
glm::mat4 proj = glm::mat4(1.0);
glm::mat4 view = glm::mat4(1.0f);

int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));
	if (!IGlib::init("../shaders_P2/shader.v6.vert", "../shaders_P2/shader.v6.frag"))
		return -1;

	view[3].z = -8;

	//CBs
	IGlib::setResizeCB(resizeFunc);
	IGlib::setIdleCB(idleFunc);
	IGlib::setKeyboardCB(keyboardFunc);
	IGlib::setMouseCB(mouseFunc);

	//Se ajusta la cámara
	//Si no se da valor se cojen valores por defecto
	glm::mat4 view = glm::mat4(1.0);
	view[3].z = -6;


	float f = 1.0f / tan(3.141592f / 6.0f);
	float far = 100.0f;
	float near = 0.1f;

	proj[0].x = f;
	proj[1].y = f;
	proj[2].z = (far + near) / (near - far);
	proj[2].w = -1.0f;
	proj[3].z = (2.0f * far * near) / (near - far);
	proj[3].w = 0.0f;
	IGlib::setProjMat(proj);
	IGlib::setViewMat(view);


	sacarpuntos();
	
	//Creamos el objeto que vamos a visualizar
	/*objId = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex,
		cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);*/
	objId = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex,
		cubeVertexPos, cubeVertexColor, lolnormal, cubeVertexTexCoord, tangent);
	IGlib::addColorTex(objId, "../img/color2.png");
	IGlib::addSpecularTex(objId, "../img/specMap.png");
	IGlib::addEmissiveTex(objId, "../img/emissive.png");
	IGlib::addNormalTex(objId, "../img/normal.png");

	glm::mat4 modelMat = glm::mat4(1.0f);
	IGlib::setModelMat(objId, modelMat);

	//CBs
	IGlib::setIdleCB(idleFunc);
	IGlib::setResizeCB(resizeFunc);
	IGlib::setKeyboardCB(keyboardFunc);
	IGlib::setMouseCB(mouseFunc);
	IGlib::setMouseMoveCB(mouseMotionFunc);

	
	//Mainloop
	IGlib::mainLoop();
	IGlib::destroy();
	return 0;
}

void resizeFunc(int width, int height)
{
	float aspectratio = float(width) / float(height);
	proj[0].x = 1.0 / (aspectratio * (tan(M_PI / 6)));
	proj[1].y = 1.0 / (tan(M_PI / 6));

	IGlib::setProjMat(proj);

}

void idleFunc()
{
	glm::mat4 modelMat(1.0f);
	static float angle = 0.0f;
	angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.001f;

	modelMat = glm::rotate(modelMat, angle, glm::vec3(1.0f, 1.0f, 0.0f));
	//modelMat = glm::scale(modelMat, glm::vec3(1, 10, 10));

	IGlib::setModelMat(objId, modelMat);
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

	if (button == 2) std::cout << "de la derecha del ratón " << std::endl; {
		rotation = glm::rotate(rotation, 0.1f, glm::vec3(0.0, 1.0, 0.0));
	}

	std::cout << "en la posición " << x << " " << y << std::endl << std::endl;
	view = rotation * view;
	IGlib::setViewMat(view);
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
		std::cout << " P " << j << " " << lolnormal[k] << " " << lolnormal[k + 1] << " " << lolnormal[k + 2] << std::endl;

		k = k + 3;
	}
}

