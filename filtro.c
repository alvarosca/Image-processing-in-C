#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#include "pixel.h"
#include "imagen.h"
#include "filtro.h"


struct struct_filtro{
        char* nombre;
        funcion_t funcion;
        int nro_parametros;
};


filtro_t filtros[]={
        {"invertir",    filtro_invertir,        0},
        {"saturar",     filtro_saturar,         1},
        {"gama",        filtro_cambiar_gama,    1},
        {"brillo",      filtro_cambiar_brillo,  1},
        {"contraste", filtro_cambiar_contraste, 1},
        {"mezclar",     filtro_mezclar,         1},
        {"sepia",       filtro_sepia,           0},
        {"monocromo",   filtro_monocromo,       1},
        {"toaster",     filtro_toaster,         0},
        {"valencia",    filtro_valencia,        0}
};


filtro_t* crear_filtro(char* nombre, funcion_t funcion, int nro_parametros ){
        filtro_t* filtro=malloc(sizeof(filtro_t));
        if(filtro==NULL)
                return NULL;

	filtro->nombre=malloc( (strlen(nombre) + 1 )*sizeof(char) );
	if(filtro->nombre==NULL)
		return NULL;
  
        strcpy(filtro->nombre, nombre);
	filtro->funcion=funcion;
	filtro->nro_parametros=nro_parametros;	

        return filtro;
}
void destruir_filtro(filtro_t* filtro){
	free(filtro->nombre);
	free(filtro);
}
filtro_t* cadena_a_filtro(const char *nombre){
        size_t nro_filtros=sizeof(filtros)/sizeof(filtro_t);
        for(size_t i=0; i<nro_filtros; i++)
                if( !strcmp(nombre, filtros[i].nombre) ){
                        return crear_filtro(filtros[i].nombre, 
					    filtros[i].funcion, 
					    filtros[i].nro_parametros );
                }
        return NULL;
}
int numero_de_parametros(filtro_t* filtro){
        return filtro->nro_parametros;
}
char* nombre_filtro(filtro_t* filtro){
        return	filtro->nombre;
}
void aplicar_filtro(imagen_t* imagen, filtro_t* filtro, int parametro){
        for(size_t fil=0; fil<imagen_alto(imagen); fil++)
                for(size_t col=0; col<imagen_ancho(imagen); col++){
                        pixel_t pixel=devolver_pixel(imagen, fil, col);
                        modificar_pixel(imagen, fil, col, filtro->funcion(pixel, parametro) );
                }
}


pixel_t filtro_invertir(pixel_t p, int _){
    return pixel_desde_rgb(255 - pixel_get_rojo(p), 255 - pixel_get_verde(p), 255 - pixel_get_azul(p));
}
pixel_t filtro_saturar(pixel_t p, int porcentaje){
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    s *= (1 + porcentaje / 100.0);

    if(s < 0)
        s = 0;
    else if(s > 1)
        s = 1;

    return pixel_desde_hsv(h, s, v);
}
pixel_t filtro_cambiar_gama(pixel_t p, int desplazamiento){
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    h = (h + desplazamiento) % 360;

    return pixel_desde_hsv(h, s, v);
}
pixel_t filtro_cambiar_brillo(pixel_t p, int porcentaje){
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    v += porcentaje / 100.;

    if(v < 0)
        v = 0;
    else if(v > 1)
        v = 1;

    return pixel_desde_hsv(h, s, v);
}
pixel_t filtro_cambiar_contraste(pixel_t p, int porcentaje){
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    v *= 1 + porcentaje / 100.;

    if(v < 0)
        v = 0;
    else if(v > 1)
        v = 1;

    return pixel_desde_hsv(h, s, v);
}
pixel_t filtro_mezclar(pixel_t p, int color){
    pixel_t c = color;

    return pixel_desde_rgb(
        (pixel_get_rojo(p) + pixel_get_rojo(c)) / 2,
        (pixel_get_verde(p) + pixel_get_verde(c)) / 2,
        (pixel_get_azul(p) + pixel_get_azul(c)) / 2);
}
pixel_t filtro_sepia(pixel_t p, int _){
    componente_t r = pixel_get_rojo(p);
    componente_t v = pixel_get_verde(p);
    componente_t a = pixel_get_azul(p);

    int rn = r * 0.393 + v * 0.769 + a * 0.189;
    int vn = r * 0.349 + v * 0.686 + a * 0.168;
    int an = r * 0.272 + v * 0.534 + a * 0.131;

    if(rn > 255) rn = 255;
    if(vn > 255) vn = 255;
    if(an > 255) an = 255;

    return pixel_desde_rgb(rn, vn, an);
}
pixel_t filtro_monocromo(pixel_t p, int porcentaje_umbral){
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    return (v > porcentaje_umbral / 100.) ? pixel_desde_rgb(255, 255, 255) : pixel_desde_rgb(0, 0, 0);
}
pixel_t filtro_toaster(pixel_t p, int _){
    p = filtro_mezclar(p, pixel_desde_rgb(51, 0, 0));

    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    v = v * 1.5 + 0.2;
    s *= 0.8;
    h = (h + 20) % 360;

    if(v > 1)
        v = 1;

    return pixel_desde_hsv(h, s, v);
}
pixel_t filtro_valencia(pixel_t p, int _){
    float r = pixel_get_rojo(p) / 100.;
    float ve = pixel_get_verde(p) / 100.;
    float a = pixel_get_azul(p) / 100.;

    r = 0.23 + r - 2 * 0.23 * r;
    ve = 0.01 + ve - 2 * 0.01 * ve;
    a = 0.22 + a - 2 * 0.22 * a;

    if(r > 1) r = 1;
    if(ve > 1) ve = 1;
    if(a > 1) a = 1;

    if(r < 0) r = 0;
    if(ve < 0) ve = 0;
    if(a < 0) a = 0;

    p = filtro_mezclar(p, pixel_desde_rgb(r * 255, ve * 255, a * 255));

    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    v = v * 1.08 - 0.08;
    if(v > 1)
        v = 1;
    if(v < 0)
        v = 0;

    p = pixel_desde_hsv(h, s, v);
    pixel_t c = filtro_sepia(p, 0);

    return pixel_desde_rgb(
        pixel_get_rojo(p) * 0.92 + pixel_get_rojo(c) * 0.08,
        pixel_get_verde(p) * 0.92 + pixel_get_verde(c) * 0.08,
        pixel_get_azul(p) * 0.92 + pixel_get_azul(c) * 0.08);
}

