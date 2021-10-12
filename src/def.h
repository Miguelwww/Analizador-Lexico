/*
    Fichero de definiciones de componentes léxicos de D
*/

#ifndef DEF_H
#define DEF_H

/*
    Caracteres como ; son ASCII, por lo que no hace falta usar DEFINE
    Empezaremos en 256 para evitar colisiones con ASCII de 8 bits

    - Los identificadores tienen ID 300
    - Las palabras reservadas tienen ID 4XX
    - Los tipos de datos primitivos tienen ID 5XX
    - Los tokens (siempre que no sean ASCII) tienen ID 6XX
    - Todo lo relacionado con comentarios tiene id 7XX
*/

#define ID			300

#define KW_IMPORT	400
#define KW_IF		401
#define KW_ELSE		402
#define KW_SWITCH	403
#define KW_CASE		404
#define KW_FOR		405
#define KW_DO		406
#define KW_WHILE     407
#define KW_FOREACH   408
#define KW_CAST      409
#define KW_EXCEPTION 410
#define KW_RETURN    411
#define KW_TRY       412
#define KW_CATCH     413
#define KW_WRITEFLN  414
#define KW_INT       450
#define KW_DOUBLE    451
#define KW_VOID      452

#define INT          501
#define STRING       502
#define INT_BINARY   511
#define INT_OCTAL    512
#define INT_HEX      513
#define DECIMAL      514

#define EQUALS       600
#define LESS_EQUALS  601
#define MORE_EQUALS  602
#define MULT_EQUALS  603
#define DIV_EQUALS   604
#define INC_ONE      605
#define DEC_ONE      606
#define PLUS_EQUALS  608

#define BC_START     700 // Block comment start
#define BC_END       701 // Block comment end
#define NBC_START    702 // Nested block comment start
#define NBC_END      703 // Nested block comment start
#define COMMENT      704

#endif
