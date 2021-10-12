#include <stdio.h>

#include "analizador_sintactico.h"
#include "analizador_lexico.h"
#include "gestor_errores.h"

int main( int argc, char *argv[] ) {

    if ( argc != 2 ) {
        errParametrosIniciales( argv[0] );
    }

    crearAnalizadorLexico( argv[1] );

    // Llamamos al sintáctico ( recuerda que solo imprime la salida del sintactico )
    analizarSintacticamente( );

    // Limpiamos
    destruirAnalizadorLexico( );

    return 0;
}
