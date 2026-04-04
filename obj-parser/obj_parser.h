#ifndef OBJ_PARSER_H_
#define OBJ_PARSER_H_
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "stb_ds.h"

#define SV_fmt "%.*s"
#define SV_arg(x) (x).count, (x).buffer

typedef struct SV{
    char* buffer;
    int count;
}SV;



typedef enum tokenKind{
    TK_INVALID,
    TK_COMMENT,
    TK_SYMBOL,
    TK_FLOAT,
    TK_NUMBER,
    TK_BACKSLASH,
    TK_FORWARDSLASH
} tokenKind;

typedef struct tt{
    SV data;
    tokenKind kind;
    int line;
    int col;
}token;

SV sv(char* stringLitrel);
int readFile(const char* file_path, char** buffer, long *buffer_size);
void sv_chop_left(SV* sv, size_t n);
int isSeprateNum(int c);
SV sv_chop_by_delmit(SV *sv, const char delmt);
SV sv_chop_by_type(SV *sv, int(*istype)(int c));
#define DEBUG 0
void parse_obj(SV obj, token** tokenArr);
#define KIND_NAME_CAP 32
void getTokenKindName(tokenKind kind, char buff[KIND_NAME_CAP]);








#endif // OBJ_PARSER_H_