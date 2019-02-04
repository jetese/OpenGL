# OpenGL / GLSL (Shaders)

Compuesto de 4 prácticas de OpenGL y GLSL

## Practica 1 Introduccion a GLSL
El objetivo de esta práctica es entender los fundamentos de programación de “shaders”
* Implementación de un Shader básico
* Color por vertice
* Acceso a texturas
* Normales
* Matriz de proyección que conserva el aspect ratio
* Añadir elementos a la escena
* Control de cámara con teclado y giro con ratón
* Movimiento de los objetos de la escena
* Crear modelo y aladir a la escena definiendo sus vértices

Mover con wasd, ratón con botón derecho para vista


## Practica 2 Iluminación en GLSL
El objetivo de esta práctica es aplicar las técnicas de iluminación en un cauce gráfico real.
Se repasarán conceptos como: modelos de iluminación, modelos de sombreado, tipos luces… 

* Sombreado de Goureau
* Sombreado de Phong
* Texturas
* Iluminar objeto con luz direccional y luz focal
* Atenuar intensidad lumínica en función de la distancia
* Implementado técnica de Bump Mapping
* Calcular normales y tangentes de un modelo cargado desde fichero


## Practica 3 Introducción a la programación en OpenGL
El objetivo de esta práctica es programar la etapa de aplicación utilizando el API de OpenGL 3.3.

* Creación del contexto de OpenGL
* Configuración del cauce
* Carga de modelo
* Renderizado del modelo
* Ajuste del viewport
* Animación
* Texturas
* Modificar propriedades de (intensidad y posición) de la luz a través de teclado
* Definir matriz de proyección que conserve el aspect ratio cuando cambia el aspect ration
* Añadir cubos a la escena con VAO
* Control de cámara con el teclado
* Mejorar el comportamiento de las texturas utilizando un filtro anisotrópico
* Implementar funcionalidad de las prácticas 1 y 2

## Practica 4 Aumento de realismo - Post-Proceso (MRT)
	El objetivo de esta práctica es entender que los métodos de iluminación local no pueden simular muchos de los comportamientos típicos de la luz. La posibilidad de renderizar sobre una textura que se pueda utilizar posteriormente en la escena nos permite diseñar técnicas que suplan de forma más o menos efectiva alguna de estas carencias.
	
* Renderiza cuadrado sobre el plano de proyección
* Motion Blur
* Gaussian Blur
* Depth of field
* Controlar parámetros de Motion Blur a través de teclado
* Controlar parámetros del DOF por teclado (distancia focal y distancia de desenfoque máximo)
* Utilizar el buffer de profundidad para controlar el DOF
* Subir nuesvas máscaras de convolución a través del DOF
* Concatena varios filtros Gausianos.
* Concatena varios post-proceso distintos
* Implementa funcionalidad de las prácticas 1,2 y 3
