#ifndef OPCION_H
#define OPCION_H

typedef struct struct_opcion opcion_t;

typedef struct struct_opciones opciones_t;


//Funciones para el procesado de las opciones//
opciones_t* crear_opciones_vacia(void);

void destruir_opciones(opciones_t* ops);

bool agregar_opcion(int argc, char* argv[argc], size_t arg_i, opciones_t* ops );

void devolver_entrada_salida(opciones_t* ops, size_t* input_index, size_t* output_index);


//Funciones para la utilización de las opciones
imagen_t* aplicar_opcion(imagen_t* imagen, char* argv[], opciones_t* ops, size_t nro_opcion );

size_t nro_opciones(opciones_t* ops);

size_t nro_argumentos_opcion(opciones_t* ops, size_t nro_opcion);

char* nombre_opcion(opciones_t* ops, size_t nro_opcion);

size_t indice_opcion(opciones_t* ops, size_t nro_opcion);


//Funciones relacionadas a las opciones disponibles
imagen_t* crop(imagen_t* imagen, char* argumento1, char* _);

imagen_t* horizontal(imagen_t* imagen, char* _ , char* __);

imagen_t* vertical(imagen_t* imagen, char* _ , char* __);

imagen_t* filtrar(imagen_t* imagen, char* argumento1 , char* argumento2);

imagen_t* void_f(imagen_t* imagen , char* _, char* __ );


#endif