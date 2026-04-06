mkdir .\build

call gcc -ggdb -Wall -Wextra main.c .\src\include\obj_parser.c -I thirdparty -I src\include -L src\lib -lSDL2 -lSDL2main -lmingw32 -lsetupapi -lole32 -limm32 -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lshell32 -lversion -limm32 -loleaut32 -lwinmm -o .\build\main.exe