#define STB_DS_IMPLEMENTATION
#include "obj_parser.h"


SV sv(char* stringLitrel){
    return (SV){
        .buffer = strdup(stringLitrel),
        .count = strlen(stringLitrel)
    };
}

int readFile(const char* file_path, char** buffer, long *buffer_size){
    FILE* fd = fopen(file_path, "r");
    int ret;
    ret = fseek(fd, 0L, SEEK_END);
    if (ret < 0) return ret;
    long size = ftell(fd);
    if (size < 0) return ret;
    ret = fseek(fd, 0L, SEEK_SET);
    if (ret < 0) return ret;
    long idx = 0;
    *buffer = calloc(size , sizeof(char));
    *buffer_size = size;
    while((ret = fgetc(fd)) != EOF){
        if (isascii((int)ret)){
            (*buffer)[idx] = ret;
            idx++;
            assert(idx <= *buffer_size);
        }
    }

    return fclose(fd);
}

int isNumSapratore(int c){
    return (c == '/' || c == ' ' || c == '\n');
}

int isFloat(SV obj){
    size_t i = 0;
    int have_dot = 0;
    if (obj.buffer[0] == '-' || ((uint8_t)(obj.buffer[0]) > 47 && (uint8_t)(obj.buffer[0]) < 58)){
        while(obj.buffer[i] != ' ' && obj.buffer[i] != '\n' && obj.count >  i && obj.buffer[i] != '/'){
            have_dot += (obj.buffer[i] == '.');
            i++;
        }
    }

    return (have_dot == 1);
}

void sv_chop_left(SV* sv, size_t n){
    if (n > sv->count) n = sv->count;
        sv->buffer += n;
        sv->count -= n;
}

SV sv_chop_by_delmit(SV *sv, const char delmit){
    size_t i = 0;
    while(i < sv->count && sv->buffer[i] != delmit){
        i++;
    }

    if(i < sv->count){
        SV res = {
            .buffer = sv->buffer,
            .count = i
        };

        sv_chop_left(sv, i);
        return res;
    }


}


SV sv_chop_by_type(SV *sv, int (*istype)(int c)){
    size_t i = 0;
    while(i < sv->count && !istype(sv->buffer[i])){
        i++;
    }

    if(i < sv->count){
        SV res = {
            .buffer = sv->buffer,
            .count = i
        };

        sv_chop_left(sv, i);
        return res;
    }

}

void parse_obj(SV obj, token** tokenArr){
    int line = 1;
    int col = 1;


    while(obj.count > 0){
        // find comments
        if (strncmp(obj.buffer, "#", 1) == 0){
            SV comment = sv_chop_by_delmit(&obj, '\n');
            token tk = {
                .data = comment,
                .kind = TK_COMMENT,
                .line = line,
                .col = col
            };
            arrput(*tokenArr, tk);
            if (DEBUG) printf("%.*s\n", comment.count, comment.buffer);
            
            col += comment.count;
        }

        // find verteces
        else if (strncmp(obj.buffer, "v ", 2) == 0){
            SV symbol = sv_chop_by_delmit(&obj, ' ');
            
            token tk = {
                .data = symbol,
                .kind = TK_SYMBOL,
                .line = line,
                .col = col
            };
            arrput(*tokenArr, tk);
            if (DEBUG) printf("%.*s\n", symbol.count, symbol.buffer);
            col += 1;
        }

        // find floating point number 
        else if (isFloat(obj)){
            SV floatNum = sv_chop_by_type(&obj, isNumSapratore);
            
            token tk = {
                .data = floatNum,
                .kind = TK_FLOAT,
                .line = line,
                .col = col
            };
            arrput(*tokenArr, tk);
            if (DEBUG) printf("%.*s\n", floatNum.count, floatNum.buffer);
            
            col += floatNum.count;
        }

        // find faces
        else if (strncmp(obj.buffer, "f ", 2) == 0){
            SV symbol = sv_chop_by_delmit(&obj, ' ');
            
            token tk = {
                .data = symbol,
                .kind = TK_SYMBOL,
                .line = line,
                .col = col
            };
            arrput(*tokenArr, tk);
            if (DEBUG) printf("%.*s\n", symbol.count, symbol.buffer);
            col += 1;
        }

        // find numers
        else if ((uint8_t)(obj.buffer[0]) > 47 && (uint8_t)(obj.buffer[0]) < 58){
            SV number = sv_chop_by_type(&obj, isNumSapratore);
            
            token tk = {
                .data = number,
                .kind = TK_NUMBER,
                .line = line,
                .col = col
            };
            arrput(*tokenArr, tk);
            if (DEBUG) printf("%.*s\n", number.count, number.buffer);
            col += number.count;
        }
        else if(obj.buffer[0] == '/'){
            SV forwardSalsh = sv("/");
            token tk = {
                .data = forwardSalsh,
                .kind = TK_FORWARDSLASH,
                .line = line,
                .col = col
            };
            arrput(*tokenArr, tk);
            if (DEBUG) printf("%.*s\n", forwardSalsh.count, forwardSalsh.buffer);
            col += forwardSalsh.count;
            obj.buffer += 1;
            obj.count -= 1;

        }
        else if(obj.buffer[0] == ' '){
            obj.buffer += 1;
            obj.count -= 1;
            col += 1;
        }
        else if (obj.buffer[0] == '\n'){
            obj.buffer += 1;
            obj.count -= 1;
            line += 1;
            col = 1;
        }
        else{
            SV invalid = sv_chop_by_type(&obj, isspace);
            
            token tk = {
                .data = invalid,
                .kind = TK_INVALID,
                .line = line,
                .col = col
            };
            arrput(*tokenArr, tk);
            if (DEBUG) printf("%.*s\n", invalid.count, invalid.buffer);
            col += invalid.count;
        }
    }
}



void getTokenKindName(tokenKind kind, char buff[KIND_NAME_CAP]){
    switch(kind){
    case TK_INVALID:{
        memset(buff, 0, KIND_NAME_CAP);
        memcpy(buff, "(INVALID)",strlen("(INVALID)"));
    }break;
    case TK_COMMENT:{
        memset(buff, 0, KIND_NAME_CAP);
        memcpy(buff, "(COMMENT)",strlen("(COMMENT)"));
    }break;
    case TK_SYMBOL:{
        memset(buff, 0, KIND_NAME_CAP);
        memcpy(buff, "(SYMBOL)",strlen("(SYMBOL)"));
    }break;
    case TK_FLOAT:{
        memset(buff, 0, KIND_NAME_CAP);
        memcpy(buff, "(FLOAT)",strlen("(FLOAT)"));
    }break;
    case TK_NUMBER:{
        memset(buff, 0, KIND_NAME_CAP);
        memcpy(buff, "(NUMBER)",strlen("(NUMBER)"));
    }break;
    case TK_BACKSLASH:{
        memset(buff, 0, KIND_NAME_CAP);
        memcpy(buff, "(BACKSLASH)",strlen("(BACKSLASH)"));
    }break;
    case TK_FORWARDSLASH:{
        memset(buff, 0, KIND_NAME_CAP);
        memcpy(buff, "(FORWARDSLASH)",strlen("(FORWARDSLASH)"));
    }break;
    }
}