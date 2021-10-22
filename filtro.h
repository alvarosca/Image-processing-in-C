#ifndef FILTRO_H
#define FILTRO_H

typedef pixel_t (*funcion_t)(pixel_t, int);

typedef struct struct_filtro filtro_t;


//Funciones para la utilización de los filtros
filtro_t* cadena_a_filtro(const char *nombre);

void destruir_filtro(filtro_t* filtro);

int numero_de_parametros(filtro_t* filtro);

char* nombre_filtro(filtro_t* filtro);

void aplicar_filtro(imagen_t* imagen, filtro_t* filtro, int parametro);


//Funciones de los filtros disponibles
pixel_t filtro_invertir(pixel_t p, int _);

pixel_t filtro_saturar(pixel_t p, int porcentaje);

pixel_t filtro_cambiar_gama(pixel_t p, int desplazamiento);

pixel_t filtro_cambiar_brillo(pixel_t p, int porcentaje);

pixel_t filtro_cambiar_contraste(pixel_t p, int porcentaje);

pixel_t filtro_mezclar(pixel_t p, int color);

pixel_t filtro_sepia(pixel_t p, int _);

pixel_t filtro_monocromo(pixel_t p, int porcentaje_umbral);

pixel_t filtro_toaster(pixel_t p, int _);

pixel_t filtro_valencia(pixel_t p, int _);

#endif