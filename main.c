#include <SDL2/SDL.h>
#include <stdio.h>
#include <assert.h>
#undef main


#define WINDOW_TITLE "ROTATING CUBE"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WHITE 0xfff0e9e9
#define BLACK 0Xff0f0e0e
#define HEAVENLY 0xfffd9564
#define BLUE 0xffff0000
#define GREEN 0xff00ff00
#define RED 0xff0000ff
#define ORANGE 0xff0b80e0
#define RED_COMP(hex) (hex >> 16 & 0xff)
#define GREEN_COMP(hex) (hex >> 8 & 0xff)
#define BLUE_COMP(hex) (hex >> 0 & 0xff)
#define ALFA(hex) (uint8_t)(hex >> 24 & 0xff)
#define MIN(n1, n2) (n1 <= n2 ? n1 : n2)
#define MAX(n1, n2) (n1 >= n2 ? n1 : n2)
#define UNIMPLEMENTED(msg) {fprintf(stderr, msg); exit(1);}
#define FPS 60


typedef struct Canvac{
    SDL_Rect rect;
    size_t stride;
    uint32_t *pixels;
} Canvac;

typedef struct vec2i{
    int x, y;
}vec2i;

typedef struct vec2f{
    float x, y;
}vec2f;

typedef struct vec3f{
    float x, y, z;
}vec3f;

vec2f normalize(vec2i vec){
    // TODO: you don't normalize correctlly here
    float x = (float)vec.x * 2 / WINDOW_WIDTH -1;
    float y = (float)vec.y * 2 / WINDOW_HEIGHT -1;
    
    return (vec2f){x, y};
}

vec2i denormalize(vec2f vec){
    float x = ((vec.x + 1) * (WINDOW_WIDTH / 2));
    float y = ((0 - vec.y + 1) * (WINDOW_HEIGHT / 2));
    return (vec2i){x, y};
}

vec2f project2d(vec3f vec){
    float z = (vec.z + 1) / 2;
    return (vec2f){
        .x = vec.x / z,
        .y = vec.y / z
    };
}

vec3f rotate_xz(vec3f vec, float angle){
    float c = SDL_cosf(angle);
    float s = SDL_sinf(angle);
    return (vec3f){
        .x = vec.x*c-vec.z*s,
        .y = vec.y,
        .z = vec.x*s+vec.z*c,
    };
}

Canvac new_Canvac(uint32_t *pixels, int32_t x, int32_t y,
                    size_t width, size_t height, size_t stride){
    Canvac c = {
        .pixels = pixels,
        .rect.x = x, .rect.y = y,
        .rect.w = width,
        .rect.h = height,
        .stride = stride
    };
    return c;
}


void CanvacDrawLine(Canvac canv, vec2i vec1, vec2i vec2 , uint8_t thickness, uint32_t color){
    int32_t minx = MIN(vec1.x, vec2.x);
    int32_t miny = MIN(vec1.y, vec2.y);
    int32_t maxx = MAX(vec1.x, vec2.x);
    int32_t maxy = MAX(vec1.y, vec2.y);

    if (minx < canv.rect.x || 
        miny < canv.rect.y || 
        maxx > canv.rect.w + canv.rect.x || 
        maxy > canv.rect.h + canv.rect.y) 
        { return;}

        double ExpectedArea = (double)SDL_sqrt(SDL_pow(maxx - minx, 2) + SDL_pow(maxy - miny, 2)) * 0.5f * thickness * 0.5f;
    for(int32_t x = minx; x <= maxx; x++){
        for(int32_t y = miny; y <= maxy; y++){
            if (vec1.x == vec2.x){
                if(y <= maxy && y >= miny && vec1.x == x){
                    int32_t index = (x - canv.rect.x) + (y - canv.rect.y) * canv.stride;
                    canv.pixels[index] = color;
                }
            }
            else{
                double Area = (double)SDL_abs(vec1.x * (vec2.y - y) + vec2.x *(y - vec1.y) + x *(vec1.y - vec2.y)) * 0.5f;
                if(Area <= ExpectedArea){
                    int32_t index = (x - canv.rect.x) + (y - canv.rect.y) * canv.stride;
                    canv.pixels[index] = color;
                }
            }
        }
    }
}

void CanvacDrawCircle(Canvac canv, vec2i vec, int32_t radius, int32_t thickness, uint32_t color){
    // get the surrounded Circle's Squire dimensions x, y
    if(thickness == 0 || thickness >= radius) thickness = radius;
    int sqx = vec.x - radius;
    sqx = MIN(MAX(sqx, canv.rect.x), canv.rect.w + canv.rect.x);
    int sqy = vec.y - radius;
    sqy = MIN(MAX(sqy, canv.rect.y), canv.rect.h + canv.rect.y);
    int sqxside = 2 * radius;
    sqxside = MIN(sqxside, canv.rect.w + canv.rect.x - sqx);
    int sqyside = 2 * radius;
    sqyside = MIN(sqyside, canv.rect.h + canv.rect.y - sqy);
    int index;
    for(int x = sqx; x < (sqx + sqxside); x++){
        for(int y = sqy; y < (sqy + sqyside); y++){
            double distance = (double)SDL_sqrt(SDL_pow(vec.x - x, 2) + SDL_pow(vec.y - y, 2));
            if(distance <= radius && distance >= radius - thickness){
                index = (x - canv.rect.x) + (y - canv.rect.y) * canv.stride;
                canv.pixels[index] = color;
            }
        }
    }
}

// Function to Draw Rectangle => thickness >
void CanvacDrawRect(Canvac canv, int rx, int ry, int32_t rw, int32_t rh, int32_t thickness, uint32_t color){
    if (thickness >= MIN(rw, rh) / 2) thickness = 0;
    // get the surrounded Circle's Squire dimensions x, y
    int32_t sqx = MIN(MAX(rx, canv.rect.x), canv.rect.w + canv.rect.x);
    int32_t sqy = MIN(MAX(ry, canv.rect.y), canv.rect.h + canv.rect.y);
    int32_t sqxside = MIN(rw, canv.rect.w + canv.rect.x - rx);
    int32_t sqyside = MIN(rh, canv.rect.h + canv.rect.y - ry);
    int index;
    for(int x = sqx; x < (sqx + sqxside); x++){
        for(int y = sqy; y < (sqy + sqyside); y++){
            if(thickness != 0 && ((sqy + thickness) <= y && y < (sqy + sqyside - thickness)) && ((sqx + thickness) <= x && x < (sqx + sqxside - thickness))) continue;
            index = (x - canv.rect.x) + (y - canv.rect.y) * canv.stride;
            canv.pixels[index] = color;
        }
    }
}

uint32_t pix_buffer[WINDOW_WIDTH*WINDOW_HEIGHT];


int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE,SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    assert(window != NULL);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
    assert(renderer != NULL);

    Canvac screen = new_Canvac(pix_buffer, 0, 0, WINDOW_WIDTH, 
                                WINDOW_HEIGHT, WINDOW_WIDTH);

    SDL_Texture *screen_texture;
    screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, 
                                                    SDL_TEXTUREACCESS_STREAMING, 
                                                    screen.rect.w,
                                                    screen.rect.h);
    assert(screen_texture != NULL);
    int screen_pitch;
    void *sheet_texture_pixels;

    int quit = 0;
    SDL_Event event;
    // float i = 1;
    vec3f cube_points[] = {
        (vec3f){-0.25, 0.25, 0.25},
        (vec3f){0.25, 0.25, 0.25},
        (vec3f){0.25, -0.25, 0.25},
        (vec3f){-0.25, -0.25, 0.25},

        (vec3f){-0.25, 0.25, -0.25},
        (vec3f){0.25, 0.25, -0.25},
        (vec3f){0.25, -0.25, -0.25},
        (vec3f){-0.25, -0.25, -0.25},
    };

    vec2i cube_lines[]= {
        (vec2i){0, 1},
        (vec2i){1, 2},
        (vec2i){2, 3},
        (vec2i){3, 0},

        (vec2i){4, 5},
        (vec2i){5, 6},
        (vec2i){6, 7},
        (vec2i){7, 4},

        (vec2i){0, 4},
        (vec2i){1, 5},
        (vec2i){2, 6},
        (vec2i){3, 7},
    };

    float angle = 0;
    while(!quit){
        SDL_SetRenderDrawColor(renderer, RED_COMP(WHITE), GREEN_COMP(WHITE), BLUE_COMP(WHITE), ALFA(WHITE));
        SDL_RenderClear(renderer);
        angle += M_PI*1/FPS;
        // for(int j = 0; j < (int)(sizeof(cube_points) / sizeof(cube_points[0])); j++){
        //     CanvacDrawCircle(screen, denormalize(project2d(rotate_xz(cube_points[j], angle))), 4, 4, RED);
        // }

        for(int j = 0; j < (int)(sizeof(cube_lines)/ sizeof(cube_lines[0])); j++){
            CanvacDrawLine(screen, denormalize(project2d(rotate_xz(cube_points[cube_lines[j].x], angle))), 
                denormalize(project2d(rotate_xz(cube_points[cube_lines[j].y], angle))) , 5, BLUE);
        }


        SDL_Delay(1000/FPS);

        // Modify the textures
        if(SDL_LockTexture(screen_texture, NULL, &sheet_texture_pixels, &screen_pitch) == 0){
            memcpy(sheet_texture_pixels, screen.pixels, screen.rect.w * screen.rect.h *sizeof(uint32_t));
            SDL_UnlockTexture(screen_texture);
        }
        SDL_RenderCopy(renderer, screen_texture, NULL, &((SDL_Rect){screen.rect.x, screen.rect.y, screen.rect.w, screen.rect.h}));


        // event Handler
        SDL_PollEvent(&event);
        switch(event.type){
            case SDL_QUIT:
            quit = 1;
            break;
            case SDL_KEYDOWN:
            switch(event.key.keysym.scancode){
                case SDL_SCANCODE_ESCAPE:
                quit = 1;
                break;
                default:
                break;
            }
            break;
            default:
            break;
        }

        SDL_RenderPresent(renderer);
        memset(pix_buffer, 0, sizeof(pix_buffer));
    }



    SDL_DestroyTexture(screen_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}