cmake_minimum_required( VERSION 3.0 )

project ( dcomp C )

set ( CMAKE_C_STANDARD 99 )

add_executable( dcomp
    src/main.c
    src/analizador_lexico.c
    src/analizador_lexico.h
    src/analizador_sintactico.h
    src/analizador_sintactico.c
    src/def.h
    src/TS.c
    src/TS.h
    src/sistema_entrada.c
    src/sistema_entrada.h
    src/gestor_errores.c
    src/gestor_errores.h
)

target_compile_options ( dcomp PRIVATE -Wall )
