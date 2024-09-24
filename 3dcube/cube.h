#pragma once

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  float x[8], y[8], z[8];
} Cube;

typedef struct {
  float x, y;
} Cord;

typedef enum { ORTHOGRAPHIC } Projection;

Cube init_cube(float x, float y, float z, float size);
void rotate_cube(Cube *cube, float radX, float radY, float radZ);
void project_cube(Cube *cube, Cord *cords, Projection p);
