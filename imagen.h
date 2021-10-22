#ifndef IMAGEN_H
#define IMAGEN_H


typedef struct struct_imagen imagen_t;


//Funciones para la utilizacion de las imágenes
imagen_t* imagen_crear(size_t ancho, size_t alto);

void imagen_destruir(imagen_t *imagen);

size_t imagen_alto(imagen_t* imagen);

size_t imagen_ancho(imagen_t* imagen);

pixel_t devolver_pixel(imagen_t* imagen, size_t fil, size_t col);

void modificar_pixel(imagen_t* imagen, size_t fil, size_t col, pixel_t pixel);


//Funciones para leer y escribir imágenes
imagen_t *imagen_leer_PPM(FILE* f);

imagen_t *imagen_leer_BMP(FILE* f);

void imagen_escribir_PPM(const imagen_t *imagen, FILE* f);

void imagen_escribir_BMP(const imagen_t *imagen, FILE *f);


//Funciones para modificar imágenes
void espejar_horizontalmente(imagen_t* imagen);

void espejar_verticalmente(imagen_t* imagen);

imagen_t *imagen_recortar(const imagen_t *imagen, size_t x0, size_t y0, size_t ancho, size_t alto);

imagen_t *imagen_clonar(const imagen_t *imagen);

#endif