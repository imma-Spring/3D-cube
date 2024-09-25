#include "cube.h"

static void matmul(float A[3][3], float B[3], float C[3]) {
  for (size_t i = 0; i < 3; ++i) {
    C[i] = 0;
    for (size_t j = 0; j < 3; ++j) {
      C[i] += A[i][j] * B[j];
    }
  }
}

Cube init_cube(float x, float y, float z, float size) {
  Cube cube;
  cube.x[0] = x;
  cube.y[0] = y;
  cube.z[0] = z;
  cube.x[1] = x + size;
  cube.y[1] = y;
  cube.z[1] = z;
  cube.x[2] = x + size;
  cube.y[2] = y;
  cube.z[2] = z + size;
  cube.x[3] = x;
  cube.y[3] = y;
  cube.z[3] = z + size;
  cube.x[4] = x;
  cube.y[4] = y + size;
  cube.z[4] = z + size;
  cube.x[5] = x + size;
  cube.y[5] = y + size;
  cube.z[5] = z + size;
  cube.x[6] = x + size;
  cube.y[6] = y + size;
  cube.z[6] = z;
  cube.x[7] = x;
  cube.y[7] = y + size;
  cube.z[7] = z;

  return cube;
}

void rotate_cube(Cube *cube, float rx, float ry, float rz) {
  float xrot[3][3] = {{1, 0, 0}, {0, cos(rx), -sin(rx)}, {0, sin(rx), cos(rx)}};
  float yrot[3][3] = {{cos(ry), 0, sin(ry)}, {0, 1, 0}, {-sin(ry), 0, cos(ry)}};
  float zrot[3][3] = {{cos(rz), -sin(rz), 0}, {sin(rz), cos(rz), 0}, {0, 0, 1}};
  float centerX = 0, centerY = 0, centerZ = 0;
  for (size_t i = 0; i < 8; ++i) {
    centerX += cube->x[i];
    centerY += cube->y[i];
    centerZ += cube->z[i];
  }

  centerX /= 8;
  centerY /= 8;
  centerZ /= 8;

  for (size_t i = 0; i < 8; ++i) {
    cube->x[i] -= centerX;
    cube->y[i] -= centerY;
    cube->z[i] -= centerZ;
  }

  for (size_t i = 0; i < 8; ++i) {
    float pos[3] = {cube->x[i], cube->y[i], cube->z[i]};
    float new_pos[3] = {0, 0, 0};
    matmul(xrot, pos, new_pos);
    cube->x[i] = new_pos[0];
    cube->y[i] = new_pos[1];
    cube->z[i] = new_pos[2];
  }

  for (size_t i = 0; i < 8; ++i) {
    float pos[3] = {cube->x[i], cube->y[i], cube->z[i]};
    float new_pos[3] = {0, 0, 0};
    matmul(yrot, pos, new_pos);
    cube->x[i] = new_pos[0];
    cube->y[i] = new_pos[1];
    cube->z[i] = new_pos[2];
  }

  for (size_t i = 0; i < 8; ++i) {
    float pos[3] = {cube->x[i], cube->y[i], cube->z[i]};
    float new_pos[3] = {0, 0, 0};
    matmul(zrot, pos, new_pos);
    cube->x[i] = new_pos[0];
    cube->y[i] = new_pos[1];
    cube->z[i] = new_pos[2];
  }

  for (size_t i = 0; i < 8; ++i) {
    cube->x[i] += centerX;
    cube->y[i] += centerY;
    cube->z[i] += centerZ;
  }
}

void move_cube(Cube *cube, float moveX, float moveY, float moveZ) {
  for (size_t i = 0; i < 8; ++i) {
    cube->x[i] += moveX;
    cube->y[i] += moveY;
    cube->z[i] += moveZ;
  }
}

Cord getPerspective(float x, float y, float z) {
  float add = z * 0.00001f;
  float w = WINDOW_WIDTH / 2.0f;
  float h = WINDOW_HEIGHT / 2.0f;

  float centeredX = x - w;
  float centeredY = y - h;

  float a = w / (w + add);
  float b = h / (h + add);

  float nx = (centeredX * a) + w;
  float ny = (centeredY * b) + h;
  Cord c = {nx, ny, x, y, z};
  return c;
}

void project_cube(Cube *cube, Cord *cords, Projection p) {
  switch (p) {
  case ORTHOGRAPHIC: {
    for (size_t i = 0; i < 8; ++i) {
      Cord c = {cube->x[i], cube->y[i], cube->x[i], cube->y[i], cube->z[i]};
      cords[i] = c;
    }
    break;
  }
  case PERSPECTIVE: {
    for (size_t i = 0; i < 8; ++i) {
      Cord c = getPerspective(cube->x[i], cube->y[i], cube->z[i]);
      cords[i] = c;
    }
    break;
  }
  }
}
