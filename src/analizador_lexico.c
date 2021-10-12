#include "analizador_lexico.h"

#include <stdlib.h>
#include <string.h>

#include "gestor_errores.h"
#include "sistema_entrada.h"
#include "TS.h"
#include "def.h"

//                                                                     VARIABLES
////////////////////////////////////////////////////////////////////////////////

abb ts;		// Tabla de simbolos
int nested;	// Contador de comentarios anidados
int linea;	// Linea actual

//                                                                     FUNCIONES
////////////////////////////////////////////////////////////////////////////////

void crearAnalizadorLexico( char *path ) {

    // Creamos tabla de símbolos
    crear_arbol( &ts );

    // Insertamos las palabras reservadas en la tabla de simbolos
    char *lexicos[9] = {
        "import", "double", "int", "while", "foreach", "void", "return", "cast", "exception"
    };
    int valores[9] = {
        KW_IMPORT, KW_DOUBLE, KW_INT, KW_WHILE, KW_FOREACH, KW_VOID, KW_RETURN, KW_CAST, KW_EXCEPTION
    };

    for ( int i = 0; i < 9; i++ ) {
        tipoelem te;
        strcpy( te.lexema, lexicos[i] );
        te.componente = valores[i];
        insertar( &ts, te );
    }

    // Creamos sistema de entrada
    crearSistemaEntrada( path ); 

    // Establecer variables iniciales
    nested = 0;
    linea = 1;
}

void _check_max_lex( tipoelem *te, int *pos ) {
	// BLOCK_SIZE porque hay que incluir \0
	if ( *pos - 1 == BLOCK_SIZE ) {
		te->lexema[(*pos)++] = '\0';
		errLexemaDemasiadoGrande( te->lexema, BLOCK_SIZE );
	}
}

void _automata_ids( tipoelem *te, int *pos ) {
    int finished = 0;

    do {
        char c = getCaracter( );

        // Contador de lineas
        if ( c == '\n' ) {
            linea++;
        }

        switch ( c ) {
            case 65 ... 90:
            case 97 ... 122:
            case 48 ... 57:
            case '_':
                te->componente = ID;
                te->lexema[(*pos)++] = c;
                break;
            default:
                lexemaTerminado( );
                finished = 1;
                break;
        }

        _check_max_lex( te, pos );
    } while ( ! finished );

    tipoelem resultadoBusqueda;
    resultadoBusqueda.componente = -1;
    buscar_nodo( ts, te->lexema, &resultadoBusqueda );
    if ( resultadoBusqueda.componente != -1 ) {
        te->componente = resultadoBusqueda.componente;
    }
}

void _automata_numeros_binarios( tipoelem *te, int *pos ) { 
    int finished = 0;
    te->componente = INT_BINARY;

    do {
        char c = getCaracter( );

        // Contador de lineas
        if ( c == '\n' ) {
            linea++;
        }

        if ( c == '0' || c == '1' || c == '_' ) {
            te->lexema[(*pos)++] = c;
        } else {
            lexemaTerminado( );
            finished = 1;
        }
    } while ( !finished );
}

void _automata_numeros( tipoelem *te, int *pos, int binario ) { // binario = 0 indica que no puede ser binario
    int finished = 0;
    int state = 0; // 0 = numero, 1 = binario; 2 = signo exponente; 3 = valor exponente
    int dotCount = 0; // Contador de puntos, sólo puede haber uno
    te->componente = INT;

    do {
        char c = getCaracter( );

        // Contador de lineas
        if ( c == '\n' ) {
            linea++;
        }

        switch (state) {

            // Leyendo número con posibilidad de ser binario
            case 0:
                switch ( c ) {
                    case 48 ... 57: // Es número
                        te->lexema[(*pos)++] = c;
                        break;

                    case '_':
                        te->lexema[(*pos)++] = c;
                        break;
                    
                    case '.': // Un punto
                        if ( dotCount == 0 ) {
                            te->lexema[(*pos)++] = c;
                            te->componente = DECIMAL;
                            dotCount++;
                        } else {
                            lexemaTerminado( );
                            finished = 1;
                        }
                        break;

                    case 'e': // Leer exponente
                        te->lexema[(*pos)++] = c;
                        state = 1;
                        break;
                    
                    case 'b':
                        te->lexema[(*pos)++] = c;
                        if ( *pos == 1 && binario == 1 ) {
                            _automata_numeros_binarios( te, pos );
                        }
                        break;

                    default: // Fin
                        if ( te->lexema[(*pos) - 1] == '.' && dotCount == 1 ) {
                            lexemaTerminado( );
                        }
                        finished = 1;
                        lexemaTerminado( );
                }
                break;
  
            case 1: // Exponente ( signo )
                if ( c == '+' || c == '-' ) {
                    te->lexema[(*pos)++] = c;
                    state = 2;
                } else {
                    lexemaTerminado( );
                    finished = 1;
                }
                break;
                
            case 2: // Exponente ( valor )
                if ( (c >= 48 && c <= 57 ) || c == '_' ) {
                    te->lexema[(*pos)++] = c;
                } else {
                    lexemaTerminado( );
                    te->componente = DECIMAL;
                    finished = 1;
                }
                break;
        }
        _check_max_lex( te, pos );
    } while ( !finished );
}

// Detecta operadores /= y comentarios
void _automata_slash( tipoelem *te, int *pos ) {
    int finished = 0;
    int state = 0;

    do {
        char c = getCaracter( );

        // Contador de lineas
        if ( c == '\n' ) {
            linea++;
        }

        switch (state) {

            // Primer caracter
            case 0:
                switch ( c ) {
                    case '/':
                        state = 1;
                        te->lexema[(*pos)++] = c;
                        te->componente = COMMENT;
                        break;
                    case '*':
                        state = 2;
                        te->lexema[(*pos)++] = c;
                        te->componente = BC_START;
                        break;
                    case '+':
                        te->lexema[(*pos)++] = c;
                        nested++;
                        te->componente = NBC_START;
                        state = 3;
                        break;
                    default:
                        lexemaTerminado( );
                        finished = 1;
                }
                break;

            // Comentario... ir a fin de línea

            case 1:
                if ( c == '\n' || c == EOF ) {
                    finished = 1;
                    break;
                }
                break;

            // Comentario de bloque

            case 2: // /*
                if ( c == '*' ) {
                    state = 21;
                }
                break; 

            case 21: // /* ... *
                if ( c == '/' ) {
                    lexemaTerminado( );
                    lexemaTerminado( );
                    finished = 1;
                } else if ( c != '*' ) {
                    state = 2;
                }
                break;
            
            // Comentario de bloque anidado

            case 3: // /+
                if ( c == '/' ) {
                    state = 31;
                } else if ( c == '+' ) {
                    state = 32;
                }
                break;
            
            case 31: // /+ ... /
                if ( c == '+' ) {
                    nested++;
                    state = 3;
                } else if ( c != '/' )  {
                    state = 3;
                }
                break;
            
            case 32: // /+ ... +
                if ( c == '/' ) {
                    nested--;
                    state = 3;

                    if ( nested == 0 ) {
                        finished = 1;
                    }
                } else if ( c != '+' )  {
                    state = 3;
                }
                break;
        }
        _check_max_lex( te, pos );
    } while ( !finished );

}

// Detecta operadores *= y fines de comentario
void _automata_asterisco( tipoelem *te, int *pos ) {
    
    char c = getCaracter( );

    // Contador de lineas
    if ( c == '\n' ) {
        linea++;
    }

    switch ( c ) {

        // Fin de comentario en bloque
        case '/':
            te->componente = NBC_END;
            te->lexema[(*pos)++] = c;
            break;

        // Operador /*
        case '=':
            te->lexema[(*pos)++] = c;
            te->componente = MULT_EQUALS;
            break;

        default:
            lexemaTerminado( );
    }
    
    _check_max_lex( te, pos );
}

// Detección de caracteres entre comillas
void _automata_caracteres( tipoelem *te, int *pos ) {
    int finished = 0;
    int state = 0;

    do {
        char c = getCaracter( );

        // Contador de lineas
        if ( c == '\n' ) {
            linea++;
        }

        switch (state) {
            case 0: // '
                te->lexema[(*pos)++] = c;
                switch ( c ) {
                    case '\'': // ''
                        finished = 1;
                        break;
                    case '\\': // '\...
                        state = 1;
                        break;
                    default: // 'x
                        state = 2;
                }
                break;
            
            case 1: // '\...
                te->lexema[(*pos)++] = c;
                state = 2;
                break;

            case 2: // 'x | '\x
                if ( c == '\'' ) {
                    te->lexema[(*pos)++] = c;
                    finished = 1;
                } else {
                    // TODO error
                }
                break;
        }

        _check_max_lex( te, pos );
    } while ( !finished );
}

// Detección de caracteres entre comillas
void _automata_comparacion_igual( tipoelem *te, int *pos ) {
    char c = getCaracter( );

    // Contador de lineas
    if ( c == '\n' ) {
        linea++;
    }

    if ( c == '=' ) { // ==
        te->lexema[(*pos)++] = c;
        te->componente = EQUALS;
    } else {
        lexemaTerminado( );
    }
}

void _automata_caracter_mas( tipoelem *te, int *pos ) {
    char c = getCaracter( );

    // Contador de lineas
    if ( c == '\n' ) {
        linea++;
    }

    switch ( c ) {
        case '+':
            te->lexema[(*pos)++] = c;
            te->componente = INC_ONE;
            break;
        
        case '=':
            te->lexema[(*pos)++] = c;
            te->componente = PLUS_EQUALS;
            break;
        
        default:
            lexemaTerminado( );
    }
}

void _automata_strings( tipoelem *te, int *pos ) {
    int finished = 0;
    int state = 0;

    do {
        char c = getCaracter( );

        // Contador de lineas
        if ( c == '\n' ) {
            linea++;
        }

        switch ( state ) {

            // Lectura de caracteres
            case 0: // "
                te->lexema[(*pos)++] = c;
                switch ( c ) {
                    case '\"': // ""
                        finished = 1;
                        te->componente = STRING;
                        break;
                    case '\\': // "\...
                        state = 1;
                        break;
                }
                break;

            // Caracteres escapados
            case 1: // "\...
                te->lexema[(*pos)++] = c;
                state = 0;
                break;
        }

        _check_max_lex( te, pos );
    } while ( !finished );
}

tipoelem siguienteComponenteLexico( ) {
    
    tipoelem te;
    int pos = 0;		// Posicion en el lexema

    // Iniciamos un lexema vacio
    te.lexema[0] = '\0';
    te.componente = -1;
    te.linea = linea;
    
    // Bucle principal
    do {
        char c = getCaracter( );

        // Contador de lineas
        if ( c == '\n' ) {
            linea++;
            te.linea = linea;
        }

        // Autómata
        switch ( c ) {
                // Lexema
                case 65 ... 90:
                case 97 ... 122:
                    te.componente = ID;
                case '_':
                    te.lexema[pos++] = c;
                    _automata_ids( &te, &pos );
                    break;
                
                // Número, podría ser binario
                case '0':
                    te.lexema[pos++] = c;
                    _automata_numeros( &te, &pos, 1 );
                    break;
                
                // Número no binario
                case 49 ... 57:
                    te.lexema[pos++] = c;
                    _automata_numeros( &te, &pos, 0 );
                    break;
                
                case '/':
                    te.lexema[pos++] = c;
                    _automata_slash( &te, &pos );
                    break;
                
                case '*':
                    te.lexema[pos++] = c;
                    _automata_asterisco( &te, &pos );
                    break;
                
                case '+':
                    te.lexema[pos++] = c;
                    _automata_caracter_mas( &te, &pos );
                    break;

                case '=':
                    te.lexema[pos++] = c;
                    _automata_comparacion_igual( &te, &pos );
                    break;

                // Caracteres
                case '\'':
                    te.lexema[pos++] = c;
                    _automata_caracteres( &te, &pos );
                    break;

                // Strings
                case '\"':
                    te.lexema[pos++] = c;
                    _automata_strings( &te, &pos );
                    break;

                // Inválidos
                case '\n':
                case ' ':
                case '\r':
                case '\t':
                    break;
                case EOF:
                    te.componente = EOF;
                    pos++;
                    break;

                // ASCII
                default:
                    te.lexema[pos++] = c;
            }
        } while ( pos == 0 );

    if ( pos == 0 ) {
        tipoelem vacio;
        vacio.componente = EOF;
        vacio.linea = 0;
        vacio.lexema[0] = '\0';
        return vacio;
    }

    // Añadimos el final de string al lexema
    te.lexema[pos++] = '\0';

    // Si el componente es de tamaño 1 guardamos su ascii
    if ( strlen( te.lexema ) == 1 && te.componente == -1 ) {
        te.componente = ( int )te.lexema[0];
    }

    // Si el componente es un identificador que no esta en la tabla
    // lo insertamos
    // if ( te.componente == ID && !es_miembro( ts, te ) ) { // (SE MODIFICO ABB PARA QUE SEA MAS RAPIDO)
    if ( te.componente == ID ) { 
        insertar( &ts, te );
    }

    // Si no es un comentario...
    if ( te.componente < BC_START || te.componente > COMMENT ) {

        // Devolvemos el componente
        return te;
    }

    
    //return te;
    return siguienteComponenteLexico( );
}

// Metodo privado pensado para ser recursivo y ayudar
// a imprimir el arbol
void _imprimir( abb A ) {
    abb ader = der( A ), aizq = izq( A ); // Obtener subarboles
    tipoelem te;

    // Si el subarbol izquierdo existe, imprimelo
    if ( !es_vacio( aizq ) ) {
        _imprimir( aizq );
    }
    
    // Imprimir este nodo
    info( A, &te );
    printf( "%d\t\033[0;32m%s\033[0;0m\n", te.componente, te.lexema );

    // Si el subarbol izquierdo existe, imprimelo
    if ( !es_vacio( ader ) ) {
        _imprimir( ader );
    }
}

void imprimirTablaSimbolos( ) {
    abb Arbol = ts;

    printf( "\n\n\033[1;29mTABLA DE SIMBOLOS\nCOMP.\tLEXEMA\033[0;0m\n_____\t______\n" );
    _imprimir( Arbol );
}

void destruirAnalizadorLexico( ) {
    destruir_arbol( &ts );
    destruirSistemaEntrada( );
}
