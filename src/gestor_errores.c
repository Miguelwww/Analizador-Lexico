#include "gestor_errores.h"

#include <stdio.h>
#include <stdlib.h>

#define RED		"\033[31;1m"
#define BLUE	"\033[34;1m"
#define RESET	".\033[0;0m\n"

void errLexemaDemasiadoGrande( char *lex, int blksize ) {
	fprintf( stderr,
		"%sEl lexema %s%s%s ha excedido el tamaño de bloque establecido (%d)%s",
		RED, BLUE, lex, RED, blksize, RESET );
	exit( 102 );
}

void errFicheroNoExiste( char *path ) {
	fprintf( stderr,
		"%sEl fichero \"%s\" no existe o no se dispone de permisos para acceder al mismo%s",
		RED, path, RESET );
	exit( 101 );
}

void errParametrosIniciales( char *progName ) {
	fprintf( stderr,
		"%sEl número de parámetros erróneo. Uso: %s FICHERO A COMPILAR%s",
		RED, progName, RESET );
	exit( 100 );
}
