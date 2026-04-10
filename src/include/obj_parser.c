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
    int i = 0;
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
    if ((int)(n) > sv->count) n = sv->count;
        sv->buffer += n;
        sv->count -= n;
}

SV sv_chop_by_delmit(SV *sv, const char delmit){
    int i = 0;
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

    return *sv;


}


SV sv_chop_by_type(SV *sv, int (*istype)(int c)){
    int i = 0;
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

    return *sv;
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

int toInt(char* strNum, size_t len){
    char* endptr;
    char tempNum[len+1];
    memcpy(tempNum, strNum, len);
    tempNum[len] = '\0';
    int i = (int)strtof(tempNum, &endptr);

    if(tempNum == endptr){
        fprintf(stderr, "toInt: No digits were found: `%s`\n", tempNum);
        exit(-1);
    }
    else if(*endptr != '\0'){
        fprintf(stderr, "toInt: Farther char after number: `%s`\n", tempNum);
        exit(-1);
    }
    return i;
}

float toFloat(char* strNum, size_t len){
    char* endptr;
    char tempNum[len+1];
    memcpy(tempNum, strNum, len);
    tempNum[len] = '\0';
    float i = strtof(tempNum, &endptr);

    if(tempNum == endptr){
        fprintf(stderr, "toFloat: No digits were found: `%s`\n", tempNum);
        exit(-1);
    }
    else if(*endptr != '\0'){
        fprintf(stderr, "toFloat: Farther char after number: `%s`\n", tempNum);
        exit(-1);
    }
    return i;
}

void getObjData(obj* model, token* tkArr){
    size_t n = arrlenu(tkArr);
    assert(n > 0);
    model->verteces = NULL;
    model->faces = NULL;
    size_t verteces_count = 0;
    size_t faces_count = 0;
    // int scale_value = 0;

    for(size_t i = 0; i < n; i++){
        
        if (strncmp(tkArr[i].data.buffer, "v", 1) == 0
         && tkArr[i].data.count == 1){
            vec3f vertex = {0};
            assert(tkArr[i+1].kind == TK_FLOAT);
            assert(tkArr[i+2].kind == TK_FLOAT);
            assert(tkArr[i+3].kind == TK_FLOAT);
            vertex.x = toFloat(tkArr[i+1].data.buffer, tkArr[i+1].data.count);
            vertex.y = toFloat(tkArr[i+2].data.buffer, tkArr[i+2].data.count);
            vertex.z = toFloat(tkArr[i+3].data.buffer, tkArr[i+3].data.count);
            arrput(model->verteces, vertex);
            verteces_count += 1;
        }
        // else if (strncmp(tkArr[i].data.buffer, "s", 1) == 0 &&
        //     tkArr[i].data.count == 1 &&
        //     strncmp(tkArr[i + 1].data.buffer, "off", 3) != 0){
        //     assert(tkArr[i + 1].kind == TK_NUMBER);
        //     scale_value = toInt(tkArr[i + 1].data.buffer, tkArr[i + 1].data.count);
        // }

        else if (strncmp(tkArr[i].data.buffer, "f", 1) == 0
         && tkArr[i].data.count == 1){
            size_t j = i + 1;
            int tempVec3i[3] = {0};

            size_t temp_idx = 0;
            while(tkArr[j].kind != TK_SYMBOL && j < n){
                if(tkArr[j - 1].kind != TK_FORWARDSLASH &&
                tkArr[j].kind == TK_NUMBER){
                    if (temp_idx > 3) {
                        j++;
                        continue;
                    }
                    assert(tkArr[j].kind == TK_NUMBER);
                    tempVec3i[temp_idx] = toInt(tkArr[j].data.buffer, tkArr[j].data.count);
                    temp_idx++;
                }
                j++;
            }
            i = j - 1;
            vec3i face = {0};
            face.x = tempVec3i[0] - 1; // the indces in .obj starts from 1
            face.y = tempVec3i[1] - 1; // the indces in .obj starts from 1
            face.z = tempVec3i[2] - 1; // the indces in .obj starts from 1
            arrput(model->faces, face);
            faces_count += 1;
            temp_idx = 0;
        }
    }
    fprintf(stdout, "[INFO] verteces count = %llu\n", verteces_count);
    // fprintf(stdout, "[INFO] scaler value = %d\n", scale_value);
    fprintf(stdout, "[INFO] faces count = %llu\n", faces_count);
    
    float normalizer_factor = 1.0f;
    for(size_t i = 0; i < arrlenu(model->verteces); i++){
        normalizer_factor = MAX(normalizer_factor, SDL_fabsf(model->verteces[i].x));
        normalizer_factor = MAX(normalizer_factor, SDL_fabsf(model->verteces[i].y));
        normalizer_factor = MAX(normalizer_factor, SDL_fabsf(model->verteces[i].z));
    }
    for (size_t i = 0; i < arrlenu(model->verteces); i++){
        model->verteces[i].x = model->verteces[i].x / normalizer_factor;
        model->verteces[i].y = (model->verteces[i].y / normalizer_factor) + PAD_Y_AXIS;
        model->verteces[i].z = (model->verteces[i].z / normalizer_factor);
    }
    fprintf(stdout, "[INFO] verteces normalized successfully = %f\n", normalizer_factor);

}

