#ifndef GESTOR_ERRORES_H
#define GESTOR_ERRORES_H 

// Lexema ha excedido BLOCK_SIZE en el analizador léxico
void errLexemaDemasiadoGrande( char *ex, int blksize );

// El sistema de entrada no tiene acceso al fichero
void errFicheroNoExiste( char *path );

// En el main los parámetros no son los adecuados
void errParametrosIniciales( char *progName );

#endif
