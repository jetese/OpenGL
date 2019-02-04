#ifndef __IG_AUXILIAR__
#define __IG_AUXILIAR__

//Carga una textura y devuelve un puntero a su ubicacion en mememoria principal
//también devuelve el tamaño de la textura (w,h)
//!!Ya implementada
unsigned char *loadTexture(const char* fileName, unsigned int &w, unsigned int &h);

//Carga un fichero en una cadena de caracteres (para subir shaders)
char *loadStringFromFile(const char *fileName, unsigned int &fileLen);


#endif