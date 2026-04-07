#ifndef OBJ_PARSER_H_
#define OBJ_PARSER_H_
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "stb_ds.h"
#include <SDL2/SDL_stdinc.h>
#define MIN(n1, n2) (n1 <= n2 ? n1 : n2)
#define MAX(n1, n2) (n1 >= n2 ? n1 : n2)
#define PAD_Y_AXIS -0.5

#define SV_fmt "%.*s"
#define SV_arg(x) (x).count, (x).buffer

typedef struct SV{
    char* buffer;
    int count;
}SV;


typedef struct vec3i{
    int x, y, z;
}vec3i;

typedef struct vec3f{
    float x, y, z;
}vec3f;

typedef struct vec2i{
    int x, y;
}vec2i;


typedef struct vec2f{
    float x, y;
}vec2f;



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

typedef struct obj{
    vec3f* verteces;
    vec3i* faces;
}obj;


SV sv(char* stringLitrel);
int readFile(const char* file_path, char** buffer, long *buffer_size);
void sv_chop_left(SV* sv, size_t n);
int isSeprateNum(int c);
SV sv_chop_by_delmit(SV *sv, const char delmt);
SV sv_chop_by_type(SV *sv, int(*istype)(int c));
int isFloat(SV obj);
#define DEBUG 0
void parse_obj(SV obj, token** tokenArr);
#define KIND_NAME_CAP 32
void getTokenKindName(tokenKind kind, char buff[KIND_NAME_CAP]);
int toInt(char* strNum, size_t len);
float toFloat(char* strNum, size_t len);
void getObjData(obj* model, token* tkArr);







#endif // OBJ_PARSER_H_