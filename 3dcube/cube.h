#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

typedef struct {
  float x[8], y[8], z[8];
} Cube;

typedef struct {
  float x, y;
  float tdx, tdy, tdz;
} Cord;

typedef enum { ORTHOGRAPHIC, PERSPECTIVE } Projection;

Cube init_cube(float x, float y, float z, float size);
void rotate_cube(Cube *cube, float radX, float radY, float radZ);
void move_cube(Cube *cube, float moveX, float moveY, float moveZ);
void project_cube(Cube *cube, Cord *cords, Projection p);
Cord getPerspective(float x, float y, float z);
