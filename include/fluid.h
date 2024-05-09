#include <stdlib.h>

#ifndef FLUID_H
#define FLUID_H

#define SIM_RES 64 // resolution
#define IX(i, j) ((i) + (SIM_RES) * (j))

typedef struct Fluid
{
    int size;
    float dt;
    float diff;
    float visc;

    float *s;
    float *density;

    float *Vx;
    float *Vy;

    float *Vx0;
    float *Vy0;
} Fluid;

Fluid *create_fluid_grid(int size, float diff, float visc, float dt);
void free_fluid_grid(Fluid *grid);
void add_density(Fluid *grid, int x, int y, float amount);
void add_velocity(Fluid *grid, int x, int y, float amountX, float amountY);
void set_bnd(int b, float *x, int N);
void lin_solve(int b, float *x, float *x0, float a, float c, int iter, int N);
void diffuse(int b, float *x, float *x0, float diff, float dt, int iter, int N);
void project(float *velocX, float *velocY, float *p, float *div, int iter, int N);
void advect(int b, float *d, float *d0, float *velocX, float *velocY, float dt, int N);
void fluid_grid_step(Fluid *grid);

#endif