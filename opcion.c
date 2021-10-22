#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "pixel.h"
#include "imagen.h"
#include "filtro.h"
#include "opcion.h"


typedef imagen_t* (*opfuncion_t)(imagen_t*, char*, char* );


//Tda opcion_t
//Debe almacenar las características de una opcion válida
struct struct_opcion{
        char* nombre;           //Debe almacenar el nombre de la opción
        opfuncion_t funcion;    //Debe almacenar la función a la que esta relacionada
        size_t nro_argumentos;  //Debe almacenar el nro de argumentos que se utilizaron al invocar dicha opción(incluye al nombre tambien)
};


//Tda opciones_t 
//Debe almacenar la lista y cantidad de opciones que se encuentran dentro de un vector de cadenas( Específicamente argv )
struct struct_opciones{     
        opcion_t* opciones;     //arreglo de opciones
        size_t* indices;        //índices donde aparecen las opciones en el vector
        size_t nro_opciones;    //cantidad de opciones almacenadas
};

//La cantidad de opciones e índices deben ser iguales al valor de 'nro_opciones'
//Se debe cumplir que: argv[ indices[i] ] = opciones[ indices[i] ].nombre 


//Tabla de busqueda con las opciones disponibles y sus parámetros.
static opcion_t opciones[]={
        {"-crop", crop, 2},
        {"-horizontal", horizontal, 1},
        {"-vertical", vertical, 1},
        {"-filter", filtrar, 3},
        {"-input", void_f, 2},
        {"-output", void_f, 2}
};


static bool _es_opcion(char* argumento);

static opcion_t* _agregar_opciont(opcion_t* opcion, size_t nro_opciones, opcion_t op_original, size_t nro_argumentos);


//Devuelve 'true' si la cadena tiene la sintaxis correspondiente a una opción
static bool _es_opcion(char* argumento){
        return argumento[0] == '-' && atoi(argumento) >= 0 ;
}
//Agrega una opción a un arreglo de 'opcion_t'
static opcion_t* _agregar_opciont(opcion_t* opcion, size_t nro_opciones, opcion_t op_original, size_t nro_argumentos){
        opcion_t* aux=realloc( opcion, (nro_opciones+1)*sizeof(opcion_t) );
        if(aux==NULL)
                return NULL;

        aux[nro_opciones].nombre=malloc( ( strlen(op_original.nombre)+1 )*sizeof(char) );
        if(aux[nro_opciones].nombre==NULL){
                free(aux);
                return NULL;
        }

        strcpy( aux[nro_opciones].nombre, op_original.nombre);
        aux[nro_opciones].funcion=op_original.funcion;
        aux[nro_opciones].nro_argumentos=nro_argumentos;

        return aux;
}


opciones_t* crear_opciones_vacia(void){
        opciones_t* ops=malloc( sizeof(opciones_t) );
        if(ops==NULL)
                return NULL;
        
        ops->opciones=NULL;
        ops->indices=NULL;
        ops->nro_opciones=0;
        return ops;
}
void destruir_opciones(opciones_t* ops){
        for(size_t i=0; i<ops->nro_opciones; i++)
                free(ops->opciones[i].nombre );
        
        free(ops->opciones);
        free(ops->indices);
        free(ops);
        return;
}
size_t nro_opciones(opciones_t* ops){
        return ops->nro_opciones;
}
size_t nro_argumentos_opcion(opciones_t* ops, size_t nro_opcion){
        return ops->opciones[nro_opcion].nro_argumentos;
}
char* nombre_opcion(opciones_t* ops, size_t nro_opcion){
        return ops->opciones[nro_opcion].nombre;
}
size_t indice_opcion(opciones_t* ops, size_t nro_opcion){
        return ops->indices[nro_opcion];
}
imagen_t* aplicar_opcion(imagen_t* imagen, char* argv[], opciones_t* ops, size_t nro_opcion ){
        char* aux1=(ops->opciones[nro_opcion].nro_argumentos>=2 )? argv[ ops->indices[nro_opcion]+1 ]:"";
        char* aux2=(ops->opciones[nro_opcion].nro_argumentos==3 )? argv[ ops->indices[nro_opcion]+2 ]:"";
         
        imagen=ops->opciones[nro_opcion].funcion(imagen, aux1, aux2);

        return imagen;
}


//Agrega en un 'opciones_t' la opción contenida en 'argv[arg_i]', asegurandose que sea válida y que 
//la cantidad de argumentos que le siguen antes de la siguiente opción sea correcta.
bool agregar_opcion(int argc, char* argv[argc], size_t arg_i, opciones_t* ops ){
        size_t n=sizeof(opciones)/sizeof(opcion_t), i, j;

        for(j=0; j<n ; j++ ){
                if( !strcmp( argv[arg_i], opciones[j].nombre ) ){
                        break;
                }
        }
        if(j==n) 
                return false;

        n=opciones[j].nro_argumentos;
        for(i=1; arg_i + i < argc && !_es_opcion(argv[arg_i+i]) ; i++ );  

        if( !strcmp(argv[arg_i], "-filter") && arg_i+1<argc ){
                filtro_t* filtro = cadena_a_filtro(argv[arg_i+1]);
                if( filtro==NULL ){
                        return false;
                }
                n=2+numero_de_parametros(filtro);
                destruir_filtro(filtro);
        }

        if( i != n )    return false;

        opcion_t* aux=_agregar_opciont(ops->opciones, ops->nro_opciones, opciones[j], n);
        if(aux==NULL){
                return false;
        }

        size_t* aux2=realloc(ops->indices, (ops->nro_opciones+1)*sizeof(size_t) );
        if(aux2==NULL){
                free(aux);
                return false;
        }

        ops->opciones=aux;
        ops->indices=aux2;
        ops->indices[ops->nro_opciones]=arg_i;

        ops->nro_opciones++;        
        return true;
}
//Devuelve por punteros los índices (almacenados en ops) a las primeras apariciones de '-input' y 'output' en argv
void devolver_entrada_salida(opciones_t* ops, size_t* input_index, size_t* output_index){
        for(size_t i=0; i<ops->nro_opciones; i++){
                if( !strcmp( ops->opciones[i].nombre, "-input" ) ){
                        *input_index=ops->indices[i];
                        break;
                }
        }
        for(size_t i=0; i<ops->nro_opciones; i++){
                if( !strcmp( ops->opciones[i].nombre, "-output" ) ){
                        *output_index=ops->indices[i];
                        break;
                }
        }
}


imagen_t* crop(imagen_t* imagen, char* argumento1, char* _){
        size_t i=0;
        size_t len=strlen(argumento1);
        size_t W=atol(argumento1);

        while( *(argumento1 + i++) != 'x' )
                if( i>=len) return NULL;

        size_t H=atol(argumento1 + i);

        while( *(argumento1 + i++) != '+' )
                if( i>=len) return NULL;

        if(H==0||W==0)
                return NULL;

        size_t X=atol(argumento1 + i);

        while( *(argumento1 + i++) != '+' )
                if( i>=len) return NULL;

        size_t Y=atol(argumento1 + i);

        imagen_t* aux = imagen_recortar(imagen, X, Y, W, H);
        if(aux==NULL)
                return false;

        imagen_destruir(imagen);
        return aux;
}
imagen_t* horizontal(imagen_t* imagen, char* _ , char* __){
        espejar_horizontalmente(imagen);
        return imagen;
}
imagen_t* vertical(imagen_t* imagen, char* _ , char* __){
        espejar_verticalmente(imagen);
        return imagen;
}
imagen_t* filtrar(imagen_t* imagen, char* argumento1 , char* argumento2){
        filtro_t* filtro=cadena_a_filtro(argumento1);
        if(filtro==NULL)
                return NULL;

        int parametro = 0;
        int n_params = numero_de_parametros(filtro);
        if(n_params == 1) {
                if( !strcmp( nombre_filtro(filtro) , "mezclar") ){
                        if( es_color_valido(argumento2)==false ){
                                destruir_filtro(filtro);
                                return NULL;
                        }
                        parametro = pixel_desde_nombre(argumento2);
                }
                else {
                        parametro = atoi(argumento2);
                }
        }

        aplicar_filtro(imagen, filtro, parametro);
        destruir_filtro(filtro);
        return imagen;
}
imagen_t* void_f(imagen_t* imagen , char* _ , char* __ ){
        return imagen;
}
