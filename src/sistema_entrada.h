#ifndef SISTEMA_ENTRADA_H
#define SISTEMA_ENTRADA_H

#include <stdio.h>

#define BLOCK_SIZE 64

// Inicializa el sistema de entrada
void crearSistemaEntrada( char* path );

// Obtiene el siguiente caracter
char getCaracter( );

// Funcion que se llama para hacer retroceder el puntero de lectura
void lexemaTerminado( );

// Funcion para liberar memoria
void destruirSistemaEntrada( );

#endif
