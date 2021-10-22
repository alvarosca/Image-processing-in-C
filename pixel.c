#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "pixel.h" 


struct struct_color{
        char* name;
        pixel_t color;
};


color_t colores[]={
        {"black",   0x000000},
        {"white",   0xFFFFFF},
        {"red",     0xFF0000},
        {"lime",    0x00FF00},
        {"blue",    0x0000FF},
        {"yellow",  0xFFFF00},
        {"cyan",    0x00FFFF},
        {"magenta", 0xFF00FF},
        {"silver",  0xC0C0C0},
        {"gray",    0x808080},
        {"maroon",  0x800000},
        {"olive",   0x808000},
        {"green",   0x008000},
        {"purple",  0x800080},
        {"teal",    0x008080},
        {"navy",    0x000080}
};


void pixel_a_hsv(pixel_t p, short *h, float *s, float *v) {
    float rp = pixel_get_rojo(p) / 255.0;
    float vp = pixel_get_verde(p) / 255.0;
    float ap = pixel_get_azul(p) / 255.0;

    float cmax = rp > vp ? rp : vp;
    cmax = cmax > ap ? cmax : ap;

    float cmin = rp < vp ? rp : vp;
    cmin = cmin < ap ? cmin : ap;

    float delta = cmax - cmin;

    if(delta == 0)
        *h = 0;
    else if(cmax == rp)
        *h = (int)(60 * (vp - ap) / delta + 360.5) % 360;
    else if(cmax == vp)
        *h = (int)(60 * (ap - rp) / delta + 120.5) % 360;
    else
        *h = (int)(60 * (rp - vp) / delta + 240.5) % 360;

    *s = cmax == 0 ? 0 : (delta / cmax);
    *v = cmax;
}
pixel_t pixel_desde_hsv(short h, float s, float v) {
    float c = s * v;
    float x = c * (1 - fabs(fmodf(h / 60.0, 2) - 1));
    float m = v - c;

    float r = 0, v_ = 0, a = 0;

    if(h < 60)
        r = c, v_ = x;
    else if(h < 120)
        v_ = c, r = x;
    else if(h < 180)
        v_ = c, a = x;
    else if(h < 240)
        a = c, v_ = x;
    else if(h < 300)
        a = c, r = x;
    else
        r = c, a = x;

    return pixel_desde_rgb((r + m) * 255, (v_ + m) * 255, (a + m) * 255);
}
pixel_t pixel_desde_rgb(componente_t rojo, componente_t verde, componente_t azul){
        return ( (pixel_t)rojo << 16 ) | ( (pixel_t)verde << 8 ) | (pixel_t)azul;
}
pixel_t pixel_desde_nombre(const char *nombre){
        size_t nro_colores=sizeof(colores)/sizeof(colores[0]); 
        for(size_t i=0; i<nro_colores; i++)
                if(!strcmp(nombre, colores[i].name))
                        return colores[i].color;

        return (pixel_t)0;
}

componente_t pixel_get_rojo(pixel_t p){
        return (componente_t)( (p & 0xff0000)>>16 );
}
componente_t pixel_get_verde(pixel_t p){
        return (componente_t)( (p & 0x00ff00)>>8 );
}
componente_t pixel_get_azul(pixel_t p){
        return (componente_t)( p & 0x0000ff );
}

bool es_color_valido(const char *nombre){
        return !(pixel_desde_nombre(nombre)==0 && strcmp(nombre, "black" )) ;
}