#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 900
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define MYCOLOR 0x25504fff
#define RAYS_NUMBER 120
#define COLOR_GRAY 0xefefefef

struct Circle {
    double x;
    double y;
    double r;
};

struct Square {
    double x;
    double y;
    double l;
};

struct Rect {
    double x;
    double y;
    double w;
    double h;
};

struct Ray {
    double x;
    double y;
    double a;
};

void FillCircle(SDL_Surface* surface, struct Circle circle, Uint32 color) {
    double radius_squared = pow(circle.r, 2);

    for (double x =  circle.x - circle.r; x <= circle.x + circle.r; x++) {
       for (double y = circle.y - circle.r; y <= circle.y + circle.r; y++) {

           double distance_squared = pow(x-circle.x, 2) + pow(y-circle.y,2);
           if (distance_squared < radius_squared) {
                //color the pixel

                SDL_Rect pixel = (SDL_Rect) {x,y, 1,1};
                SDL_FillRect(surface, &pixel, color);
            }

       }
    }
}

void FillRect(SDL_Surface* surface, struct Rect rect, Uint32 color) {
    for (double i = rect.x ; i <= rect.x + rect.w; i++) {
        for (double j = rect.y; j <= rect.y + rect.h ; j++) {
            SDL_Rect pixel = (SDL_Rect) {i,j, 1,1};
            SDL_FillRect(surface, &pixel, color);
        }
    }
}


void StoreRays(struct Circle circle, struct Ray rays[RAYS_NUMBER]) {

    for (int i = 0; i < RAYS_NUMBER; i++) {
        double angle = ((double) i / RAYS_NUMBER) * 2 * M_PI;
        struct Ray ray = {circle.x, circle.y, angle};

        rays[i] = ray;
    }

}

int queryObjectHit(double x, double y, struct Circle shadow) {
    if ( pow(x - shadow.x,2) + pow(y - shadow.y,2) <= pow(shadow.r,2) ) {
        return 1;
    } else {return 0;}
}
/*
int queryObjectHitSquare(double x, double y, struct Circle shadow) {

    double lowerExclusionX = shadow.x - shadow.r;
    double upperExclusionX = shadow.x + shadow.r;
    double lowerExclusionY = shadow.y - shadow.r;
    double upperExclusionY = shadow.y + shadow.r;

    if ((x >= lowerExclusionX && x <= upperExclusionX) && (y >= lowerExclusionY && y <= upperExclusionY)) {return 1;}
    else {return 0;}

}
*/

void FillRays(SDL_Surface* surface, struct Ray rays[RAYS_NUMBER], Uint32 color, struct Circle shadow) {

    for (int i = 0; i < RAYS_NUMBER ; i++) {
        struct Ray ray = rays[i];

        int end_of_screen = 0;
        int object_hit = 0;

        double step = 1;
        double x_draw = ray.x;
        double y_draw = ray.y;
        while (!end_of_screen && !object_hit) {


             x_draw = x_draw + step*cos(ray.a);
             y_draw = y_draw + step*sin(ray.a);

            SDL_Rect pixel = {x_draw, y_draw, 1, 1};
            SDL_FillRect(surface, &pixel, color);

            object_hit = queryObjectHit(x_draw, y_draw, shadow);

            if (x_draw < 0 || x_draw > WIDTH) {
                end_of_screen = 1;
            }
            if (y_draw < 0 || y_draw > HEIGHT) {
                end_of_screen = 1;
            }

        }

    }
}


int main(void) {

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    struct Circle circle = {200, 200, 50};
    struct Circle shadow_circle = {700, 300, 100};

    SDL_Rect erase_rect = {0,0,WIDTH, HEIGHT};

    int simulation_running = 1;
    struct Ray rays[RAYS_NUMBER];
    StoreRays(circle, rays);
    SDL_Event event;

    while (simulation_running) {

        //event checker loop
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                simulation_running = 0;
            }


            if (event.motion.state != 0) {
                circle.x = event.motion.x;
                circle.y = event.motion.y;
                StoreRays(circle, rays);
            }
        }
        SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
        FillRays(surface, rays, MYCOLOR, shadow_circle);
        FillCircle(surface, circle, MYCOLOR);
        FillCircle(surface, shadow_circle, COLOR_WHITE);
        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }

    return 0;
}