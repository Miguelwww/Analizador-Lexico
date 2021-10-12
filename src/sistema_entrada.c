#include "sistema_entrada.h"

#include "gestor_errores.h"
#include <stdio.h>

//                                                                     VARIABLES
////////////////////////////////////////////////////////////////////////////////

FILE *f; // Archivo a leer
char *lexema; // Puntero al ultimo lexema
char *caracter; // Puntero al caracter actual
char bloque0[BLOCK_SIZE + 1];
char bloque1[BLOCK_SIZE + 1]; // Bloques
char *blockPointer;
int blockCount; // Ultimo bloque (puede ser 0 o 1) y cuanta de bloques
int back = 0; // Comprueba si se ha vuelto atrás en el bloque

//                                                                     FUNCIONES
////////////////////////////////////////////////////////////////////////////////

// TODO que pasa si program < block_size
void crearSistemaEntrada( char *path ) {
    size_t size;

    // Abrir archivo
    if ( ! ( f = fopen( path, "r" ) ) ) {
		errFicheroNoExiste( path );
	}

    // Leer el primer bloque
    bloque0[BLOCK_SIZE] = EOF;
    size = fread( bloque0, sizeof( char ), BLOCK_SIZE, f);
    bloque0[size] = EOF;

    blockPointer = &bloque0[0];

    // Preparar punteros
    lexema = bloque0;
    caracter = bloque0;

    // Otras variables
    blockCount = 1;
}

char getCaracter( ) {
    size_t size;

    // Leemos el siguiente caracter del bloque 
    char readChar = *( caracter++ );

    // Si fin de bloque 
    if ( readChar == EOF ) { 

        // Cambiamos de bloque
        blockPointer = blockPointer == &bloque0[0] ? &bloque1[0] : &bloque0[0];

        // Si el nuevo bloque no ha sido cargado
        if ( ! back ) {

            // Leemos fichero
            size = fread( blockPointer, sizeof( char ), BLOCK_SIZE, f );
            blockPointer[size] = EOF;

            // Actualizamos contador
            blockCount++;

            // Avisamos
            printf( "\033[0;33mBloque lleno! cambiando bloque...\033[0;0m\n" );
        }

        // Actualizamos puntero
        caracter = blockPointer;
        back = 0;

        readChar = *( caracter++ );
    }

    return readChar;
}

void lexemaTerminado( ) {

    // Si no estamos en el primer caracter del bloque, retroceder puntero (SITUACION IMPOSIBLE, nunca se está en el primer caracter del bloque)
    if ( blockPointer != caracter ) {
        caracter--;
    } else {
        blockPointer = blockPointer == &bloque0[0] ? &bloque1[0] : &bloque0[0];
        caracter = blockPointer + BLOCK_SIZE - 1;
        back = 1;
    }

    // Ponemos el puntero a lexema al caracter
    lexema = caracter;
}

void destruirSistemaEntrada( ) {
    fclose( f );
}
