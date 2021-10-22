#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "pixel.h"
#include "imagen.h"
#include "filtro.h"
#include "opcion.h"

#define ADD_OPT_ERROR_MSG "La siguiente opción fue invocada de forma incorrecta o no existe:"
#define SAME_FILE_ERROR_MSG "El siguiente archivo se seleccionó como entrada y salida:"
#define EXT_READ_ERROR_MSG "No se pudo leer la extensión del archivo:"
#define FILE_OPEN_ERROR_MSG "No se pudo abrir al archivo:"
#define FILE_EXT_ERROR_MSG "Solo se admiten archivos de extensiones: .bmp .ppm"
#define OPT_APPLY_ERROR_MSG "Se tuvo un error al aplicar la opción número"
#define MEM_ERROR_MSG "Error de memoria"

#define MAX_EXT 5

bool extension_archivo(char* archivo_nombre, char ext[MAX_EXT]);

int main(int argc, char* argv[]){

        //Se procesan las opciones
        opciones_t* ops=crear_opciones_vacia();
        if(ops==NULL){
                fprintf(stderr, "%s\n", MEM_ERROR_MSG );
                return 1;
        }
        size_t arg_i=1;
        while(arg_i<argc){
                if( agregar_opcion(argc, argv, arg_i, ops)==false ){
                        fprintf(stderr,"%s %s\n", ADD_OPT_ERROR_MSG, argv[arg_i]);
                        destruir_opciones(ops);
                        return 1;
                }
                arg_i+=nro_argumentos_opcion(ops, nro_opciones(ops)-1 );
        }
  
        size_t input_ind=0, output_ind=0;
        devolver_entrada_salida(ops, &input_ind, &output_ind ); 
        if( input_ind!=0 && output_ind!=0 && strcmp(argv[input_ind+1], argv[output_ind+1])==0 ){
                fprintf(stderr,"%s %s\n", SAME_FILE_ERROR_MSG, argv[input_ind+1]);
                destruir_opciones(ops);
                return 1;
        }

        //Se abre el archivo de entrada
        char input_ext[MAX_EXT];
        FILE* input = stdin;
        if(input_ind!=0){
                if( extension_archivo(argv[input_ind+1], input_ext)==false ){
                        fprintf(stderr, "%s %s\n", EXT_READ_ERROR_MSG, argv[input_ind+1]);
                        destruir_opciones(ops);                              
                        return 1;
                }
                input=fopen(argv[input_ind+1], "rb");
                if(input==NULL){
                        fprintf(stderr, "%s %s\n", FILE_OPEN_ERROR_MSG,  argv[input_ind+1]);
                        destruir_opciones(ops);
                        return 1;
                }
        }

        imagen_t* img=NULL;
        if( input==stdin || !strcmp(input_ext, "ppm") ){
                img=imagen_leer_PPM( input );
        } 
        else if( !strcmp(input_ext, "bmp")  ){
                img=imagen_leer_BMP( input );
        }
        else{
                fprintf(stderr, "%s\n", FILE_EXT_ERROR_MSG);
                fclose(input);
                destruir_opciones(ops);
                return 1;
        }
        if(img==NULL){
                fprintf(stderr, "%s\n", MEM_ERROR_MSG );
                fclose(input);
                destruir_opciones(ops);
                return 1;
        }      

        //Se aplican las opciones
        for(size_t i=0; i<nro_opciones(ops) ; i++){
                imagen_t* aux=aplicar_opcion(img, argv, ops, i );
                if(aux==NULL){
                        fprintf(stderr, "%s %zd: '%s' \n", OPT_APPLY_ERROR_MSG, i+1, nombre_opcion(ops, i) );
                        destruir_opciones(ops);
                        fclose(input);
                        imagen_destruir(img);
                        return 1;
                }
                img=aux;
        }

        //Se abre el archivo de salida  
        char output_ext[MAX_EXT];
        FILE* output = stdout;
        if(output_ind!=0){
                if( extension_archivo(argv[output_ind+1], output_ext)==false ){
                        fprintf(stderr, "%s %s\n", EXT_READ_ERROR_MSG, argv[output_ind+1]);
                        fclose(input);
                        destruir_opciones(ops);
                        imagen_destruir(img);
                        return 1;
                }
                output=fopen(argv[output_ind+1], "wb");
                if(output==NULL){
                        fprintf(stderr, "%s %s\n", FILE_OPEN_ERROR_MSG, argv[output_ind+1]);
                        fclose(input);
                        destruir_opciones(ops);
                        imagen_destruir(img);
                        return 1;
                }
        }

        if( output==stdout || !strcmp(output_ext, "ppm") ){
                imagen_escribir_PPM( img, output);    
        } 
        else if( !strcmp(output_ext, "bmp")  ){
                imagen_escribir_BMP( img, output);
        }
        else{
                fprintf(stderr, "%s\n", FILE_EXT_ERROR_MSG);
                destruir_opciones(ops);
                imagen_destruir(img);
                fclose(input);
                fclose(output);
                return 1;
        }

        destruir_opciones(ops);
        imagen_destruir(img);
        fclose(input);
        fclose(output);

        return 0;
}

bool extension_archivo(char* archivo_nombre, char ext[MAX_EXT]){
        size_t len=strlen(archivo_nombre);
        size_t len2=len;
        while( archivo_nombre[-1+len--]!='.' && len!=0 );
        if(len==0)
                return false;

        len2-=len;
        if(len2>MAX_EXT+1)
                return false;

        for(size_t i=0; ( ext[i]=archivo_nombre[len+1+i] ) != '\0' ; i++ );

        return true;
}