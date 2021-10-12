#ifndef ANALIZADOR_LEXICO_H
#define ANALIZADOR_LEXICO_H

#define STANDALONE_LEX

#include "sistema_entrada.h"
#include "TS.h"

//                                                                     FUNCIONES
////////////////////////////////////////////////////////////////////////////////

// Inicializa el analizador
void crearAnalizadorLexico( char *path );

// Obtiene el siguiente componente
tipoelem siguienteComponenteLexico( );

// Libera memoria
void destruirAnalizadorLexico( );

// Imprime tabla de simbolos
void imprimirTablaSimbolos( );

#endif
