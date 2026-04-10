mkdir .\build

call gcc -o0 -ggdb -Wall -Wextra main.c .\src\include\obj_parser.c -I thirdparty -I src\include -L src\lib -l:libSDL2.a  -lmingw32 -lsetupapi -lole32 -limm32 -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lshell32 -lversion -limm32 -loleaut32 -lwinmm -o .\build\main.exe