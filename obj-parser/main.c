#include <stdio.h>
#include "obj_parser.h"


int main(int argc, char const *argv[])
{
    const char* file_path = "tank2\\IS8.obj";
    long buffer_size;
    char* buffer;
    if (readFile(file_path, &buffer, &buffer_size) != 0){
        perror("ERORR: readFile");
    }
    
    SV obj_sv = sv(buffer);
    free(buffer);

    // return 0;
    token *tkArr = NULL;
    parse_obj(obj_sv, &tkArr);
    // for (size_t i = 0; i < arrlenu(tkArr); i++){
    //     char name[32];
    //     getTokenKindName(tkArr[i].kind, name);
    //     printf(SV_fmt", kind = %14s, line = %05d, col = %02d\n", SV_arg(tkArr[i].data), name, tkArr[i].line, tkArr[i].col);
    // }

    obj model = {0};
    getObjData(&model, tkArr);
    // for(size_t i = 0; i < arrlenu(model.verteces); i++){
    //     vec3f v = model.verteces[i];
    //     printf("%f, %f, %f\n", v.x, v.y, v.z);
    // }
    // for(size_t i = 0; i < arrlenu(model.faces); i++){
    //     vec3i v = model.faces[i];
    //     printf("%d, %d, %d\n", v.x, v.y, v.z);
    // }

    return 0;
}