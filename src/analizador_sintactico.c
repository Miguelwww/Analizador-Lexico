#include "analizador_sintactico.h"

#include "analizador_lexico.h"
#include "sistema_entrada.h"

//                                                                     FUNCIONES
////////////////////////////////////////////////////////////////////////////////

void analizarSintacticamente( ) {

    tipoelem te;

    // Imprimir tabla
    imprimirTablaSimbolos( );

    // Bucle que imprime los componentes
    printf( "\033[1;29m\nANALIZADOR LEXICO\nCOMP.\tLINEA\tLEXEMA\033[0;0m\n_____\t_____\t____________\n" );
    do {
        te = siguienteComponenteLexico( );

        printf( "%d\t%d\t\033[0;32m%s\033[0;0m\n", te.componente, te.linea, te.lexema );
    } while ( te.componente != EOF );

    // Imprimir tabla
    imprimirTablaSimbolos( );
}
