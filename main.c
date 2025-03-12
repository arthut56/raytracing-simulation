#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 900
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define MYCOLOR 0xffdd00
#define RAYS_NUMBER 200
#define COLOR_GRAY 0xefefefef

struct Circle {
    double x;
    double y;
    double r;
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
    for (double i = rect.x - rect.w/2 ; i <= rect.x + rect.w/2; i++) {
        for (double j = rect.y - rect.h/2; j <= rect.y + rect.h/2 ; j++) {
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

int queryObjectHitSquare(double x, double y, struct Rect shadow) {

    double lowerExclusionX = shadow.x - shadow.w/2;
    double upperExclusionX = shadow.x + shadow.w/2;
    double lowerExclusionY = shadow.y - shadow.h/2;
    double upperExclusionY = shadow.y + shadow.h/2;

    if ((x >= lowerExclusionX && x <= upperExclusionX) && (y >= lowerExclusionY && y <= upperExclusionY)) {return 1;}

    else {return 0;}
}

int queryObjectHit(double x, double y, struct Circle shadow, struct Rect shadow_rect) {
    if ( pow(x - shadow.x,2) + pow(y - shadow.y,2) <= pow(shadow.r,2) ) {
        return 1;
    } else  {
       queryObjectHitSquare(x, y, shadow_rect);
    }
}

void FillRays(SDL_Surface* surface, struct Ray rays[RAYS_NUMBER], Uint32 color, struct Circle shadow, struct Rect rect) {

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

            object_hit = queryObjectHit(x_draw, y_draw, shadow, rect);

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

    struct Circle sun = {200, 200, 25};
    struct Circle shadow_circle = {700, 300, 50};
    struct Rect shadow_rect = {500, 500, 100, 50};

    SDL_Rect erase_rect = {0,0,WIDTH, HEIGHT};

    int simulation_running = 1;
    struct Ray rays[RAYS_NUMBER];
    StoreRays(sun, rays);
    SDL_Event event;

    while (simulation_running) {

        //event checker loop
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                simulation_running = 0;
            }
            if (event.type == SDL_KEYDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                if (event.key.keysym.sym == SDLK_a) {
                    shadow_rect.x = mouseX;
                    shadow_rect.y = mouseY;
                } else if (event.key.keysym.sym == SDLK_s) {
                    shadow_circle.x = mouseX;
                    shadow_circle.y = mouseY;
                }
            }

            if (event.motion.state != 0) {
                sun.x = event.motion.x;
                sun.y = event.motion.y;
                StoreRays(sun, rays);
            }
        }
        SDL_FillRect(surface, NULL, COLOR_BLACK);

        FillRays(surface, rays, MYCOLOR, shadow_circle, shadow_rect);
        FillCircle(surface, shadow_circle, COLOR_WHITE);
        FillRect(surface, shadow_rect, COLOR_WHITE);

        FillCircle(surface, sun, MYCOLOR);
        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }

    return 0;
}