#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "pixel.h"
#include "imagen.h"
#define MAXIMO 255 


struct struct_imagen{
    pixel_t **lienzo;
    size_t ancho, alto;
};

static pixel_t** _matriz_crear(size_t ancho, size_t alto);

static void _matriz_destruir(pixel_t** matriz, size_t alto);

static int _leer_entero(FILE* f);

static int16_t _leer_int16_little_endian(FILE *f);

static int32_t _leer_int32_little_endian(FILE *f);

static void _escribir_int16_little_endian(FILE *f, int16_t v);

static void _escribir_int32_little_endian(FILE *f, int32_t v);


static void _matriz_destruir(pixel_t** matriz, size_t alto){
        for(size_t i=0; i<alto; i++)
                free(matriz[i]);
        free(matriz);
}
static pixel_t** _matriz_crear(size_t ancho, size_t alto){
        pixel_t** matriz;

        matriz=malloc(alto*sizeof(pixel_t*));
        if(matriz==NULL)
                return NULL;

        for(size_t i=0; i<alto;i++){
                matriz[i]=malloc(ancho*sizeof(pixel_t));
                if(matriz[i]==NULL){
                        _matriz_destruir(matriz,i);
                        return NULL;
                }
        }
        return matriz;

}
static int _leer_entero(FILE* f){
        size_t i=0;
        char aux[100];

        int c=0;
        while( ( c=fgetc( f ) )!=EOF ){
                while( !isspace(c) && c!='#' ){
                        aux[i]=c;
                        i++;
			c=fgetc( f );
        	}
                if(i!=0){
                        aux[i]='\0';
                        return atoi(aux);
                }
                if(c=='#')
                        while( ( c=fgetc( f ) )!='\n' )
                                if(c==EOF)
                                        return -1;
        }
        return -1;
}
static int16_t _leer_int16_little_endian(FILE *f){
        int16_t v;
        int8_t aux[2];
        fread(&aux, sizeof(int8_t), 2, f);
        v=aux[0];
        v|=(int16_t)aux[1]<<8;
        return v;
}
static int32_t _leer_int32_little_endian(FILE *f){
        int32_t v;
        int8_t aux[4];
        fread(&aux, sizeof(int8_t), 4, f);
        v=aux[0];
        v|=(int32_t)aux[1]<<8;
        v|=(int32_t)aux[2]<<16;
        v|=(int32_t)aux[3]<<24;
        return v;
}
static void _escribir_int16_little_endian(FILE *f, int16_t v){
        int8_t elem[2];
        elem[1] = (int8_t)( ( v & 0xff00 ) >> 8 );
        elem[0] = (int8_t)( v & 0x00ff );
        fwrite(&elem, sizeof(int8_t), 2, f);
}
static void _escribir_int32_little_endian(FILE *f, int32_t v){
        int8_t elem[4];
        elem[3] = (int8_t)( ( v & 0xff000000 ) >> 24 );
        elem[2] = (int8_t)( ( v & 0x00ff0000 ) >> 16 );
        elem[1] = (int8_t)( ( v & 0x0000ff00 ) >> 8 );
        elem[0] = (int8_t)( v & 0x000000ff );
        fwrite(&elem, sizeof(int8_t), 4, f);
}


imagen_t* imagen_crear(size_t ancho, size_t alto){
        imagen_t* img;
        img=malloc(sizeof(imagen_t));
        if(img==NULL)
                return NULL;
        img->ancho=ancho;
        img->alto=alto;
        img->lienzo=_matriz_crear(ancho, alto);
        if((img->lienzo)==NULL){
                free(img);
                return NULL;
        }
        return img;
}
void imagen_destruir(imagen_t *imagen){
        _matriz_destruir(imagen->lienzo,imagen->alto);
        free(imagen);
}
size_t imagen_alto(imagen_t* imagen){
	return imagen->alto; 
}
size_t imagen_ancho(imagen_t* imagen){
	return imagen->ancho; 
}
pixel_t devolver_pixel(imagen_t* imagen, size_t fil, size_t col){
        return imagen->lienzo[fil][col];
}
void modificar_pixel(imagen_t* imagen, size_t fil, size_t col, pixel_t pixel){
        imagen->lienzo[fil][col]=pixel;
}


imagen_t* imagen_leer_PPM(FILE *f){
        imagen_t* imagen;
        char aux[10];
        fgets(aux, 10, f);
        if(strcmp(aux, "P3\n"))
                return NULL;

        size_t ancho = _leer_entero(f);
        size_t alto = _leer_entero(f);
        int max = _leer_entero(f);

        if(max != 255)
                return NULL;
        
        imagen=imagen_crear(ancho,alto);
        if(imagen==NULL)
                return NULL;

        for(int y = 0; y < alto; y++)
                for(int x = 0; x < ancho; x++) {
                        componente_t rojo = _leer_entero(f);
                        componente_t verde = _leer_entero(f);
                        componente_t azul = _leer_entero(f);
                        imagen->lienzo[y][x] = pixel_desde_rgb(rojo, verde, azul);
        }

        return imagen;
}
imagen_t *imagen_leer_BMP(FILE* f){
        char* aux;
        fread(&aux, sizeof(char), 2, f);
 
        _leer_int32_little_endian(f); //Lee 'size'       

        _leer_int16_little_endian(f);
        _leer_int16_little_endian(f);

        _leer_int32_little_endian(f); //Lee '54'
        _leer_int32_little_endian(f); //Lee '40'
        int32_t ancho = _leer_int32_little_endian(f);  //Lee 'imagen->ancho'
        int32_t alto = _leer_int32_little_endian(f); //Lee '(-1)*imagen->alto'

        _leer_int16_little_endian(f); //Lee '1'
        _leer_int16_little_endian(f); //Lee '24'
        
        _leer_int32_little_endian(f);
        _leer_int32_little_endian(f);
        _leer_int32_little_endian(f);
        _leer_int32_little_endian(f);
        _leer_int32_little_endian(f);
        _leer_int32_little_endian(f);

        size_t scanline = 3 * ancho;
        while( ( scanline % 4 )!=0 )
                scanline++;
        size_t k = scanline- 3 * ancho;

        int aux_int = (alto<0)?-1:1;
        imagen_t* imagen = imagen_crear(ancho, aux_int*alto);
        if(imagen==NULL)
                return NULL;

        uint8_t color[3];
        for(size_t fil=0; fil<imagen->alto; fil++ ){
                for(size_t col=0; col<imagen->ancho; col++){
                        fread(&color, sizeof(uint8_t), 3, f);
                        imagen->lienzo[fil][col] = pixel_desde_rgb(color[2], color[1], color[0]);
                }
                fread(&color, sizeof(uint8_t), k, f); 
        }
        return imagen;
}
void imagen_escribir_PPM(const imagen_t *imagen, FILE *f){
        fprintf(f ,"P3\n%zd %zd\n%d\n", imagen->ancho, imagen->alto, MAXIMO);
        for(size_t fil=0; fil<imagen->alto; fil++ ){
                for(size_t col=0; col<imagen->ancho; col++){
                        pixel_t pixel = imagen->lienzo[fil][col];
                        fprintf(f ,"%d %d %d\n", pixel_get_rojo(pixel), pixel_get_verde(pixel), pixel_get_azul(pixel));
                }
        }
}
void imagen_escribir_BMP(const imagen_t *imagen, FILE *f){
        size_t scanline= 3 * imagen->ancho;
        while( ( scanline % 4 )!=0 )
                scanline++;
        size_t k=scanline-3*imagen->ancho;

        int32_t size = 14 + 40 + imagen->alto * scanline;

        fprintf(f,"BM");
        _escribir_int32_little_endian(f, size);

        _escribir_int16_little_endian(f, 0);
        _escribir_int16_little_endian(f, 0);
        
        _escribir_int32_little_endian(f, 54);
        _escribir_int32_little_endian(f, 40);
        _escribir_int32_little_endian(f, imagen->ancho);
        _escribir_int32_little_endian(f, (-1)*imagen->alto);
        
        _escribir_int16_little_endian(f, 1);
        _escribir_int16_little_endian(f, 24);
        
        _escribir_int32_little_endian(f, 0);
        _escribir_int32_little_endian(f, 0);
        _escribir_int32_little_endian(f, 0);
        _escribir_int32_little_endian(f, 0);
        _escribir_int32_little_endian(f, 0);
        _escribir_int32_little_endian(f, 0);

        uint8_t color[3];
        for(size_t fil=0; fil<imagen->alto; fil++ ){
                for(size_t col=0; col<imagen->ancho; col++){
                        pixel_t pixel = imagen->lienzo[fil][col];
                        color[0]=pixel_get_azul(pixel);
                        color[1]=pixel_get_verde(pixel);
                        color[2]=pixel_get_rojo(pixel);
                        fwrite(&color, sizeof(uint8_t), 3, f);
                }
                fwrite(&color, sizeof(uint8_t), k, f); 
        }

}


imagen_t *imagen_recortar(const imagen_t *imagen, size_t x0, size_t y0, size_t ancho, size_t alto){
        if( imagen->ancho<=x0 || imagen->alto<=y0)
                return NULL;

        ancho=(x0+ancho>imagen->ancho)? imagen->ancho-x0:ancho;
        alto=(y0+alto>imagen->alto)? imagen->alto-y0:alto;

        imagen_t* recorte;
        recorte=imagen_crear(ancho,alto);
        if(recorte==NULL)
                return NULL;

        for(size_t fil=0; fil<alto; fil++){
                for(size_t col=0; col<ancho; col++){
                        recorte->lienzo[fil][col]=imagen->lienzo[fil+y0][col+x0];
                }
        }

        return recorte;
}
imagen_t *imagen_clonar(const imagen_t *imagen){
        return imagen_recortar(imagen,0,0,imagen->ancho,imagen->alto);
}
void espejar_horizontalmente(imagen_t* imagen){
        pixel_t swap;
        for(size_t fil=0; fil<imagen->alto; fil++ ){
                for(size_t i=0; i< imagen->ancho/2; i++){
                        swap=imagen->lienzo[fil][i];
                        imagen->lienzo[fil][i]=imagen->lienzo[fil][imagen->ancho-1-i];
                        imagen->lienzo[fil][imagen->ancho-1-i]=swap;
                }
        }
}
void espejar_verticalmente(imagen_t* imagen){
        pixel_t swap;
        for(size_t col=0; col<imagen->ancho; col++ ){
                for(size_t i=0; i< imagen->alto/2; i++){
                        swap=imagen->lienzo[i][col];
                        imagen->lienzo[i][col]=imagen->lienzo[imagen->alto-1-i][col];
                        imagen->lienzo[imagen->alto-1-i][col]=swap;
                }
        }
}