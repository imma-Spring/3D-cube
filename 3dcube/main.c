#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "cube.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

float sqr(float x) { return x * x; }

float distance(float x1, float y1, float x2, float y2) {
  return sqrt(sqr(x1 - x2) + sqr(y1 - y2));
}

float max(float a, float b) { return a < b ? b : a; }
float true_abs(float x) { return x < 0 ? -x : x; }

void draw_line(Cord c1, Cord c2) {
  if (true_abs(c1.x - c2.x) > true_abs(c1.y - c2.y)) {
    if (c1.x > c2.x) {
      Cord t = c1;
      c1 = c2;
      c2 = t;
    }
    float x1 = c1.x;
    float y1 = c1.y;
    float xf = c2.x;
    float yf = c2.y;
    float p = 0.25;
    float inc = (y1 - yf) / (x1 - xf) * p;
    for (float x = x1, y = y1; x <= xf; x += p, y += inc) {
      int tx = (int)x;
      int ty = (int)y;
      float dist = distance(x, y, tx, ty);
      float opacity = max(0, 1 - dist);
      glColor3f(1, 1, 1);
      glVertex2i(tx, ty);
    }
  } else {
    if (c1.y > c2.y) {
      Cord t = c1;
      c1 = c2;
      c2 = t;
    }
    float x1 = c1.x;
    float y1 = c1.y;
    float xf = c2.x;
    float yf = c2.y;
    float p = 0.25;
    float inc = (x1 - xf) / (y1 - yf) * p;
    for (float y = y1, x = x1; y <= yf; y += p, x += inc) {
      int tx = (int)x;
      int ty = (int)y;
      float dist = distance(x, y, tx, ty);
      float opacity = max(0, 1 - dist);
      glColor3f(1, 1, 1);
      glVertex2i(tx, ty);
    }
  }
}

void draw(Cord cords[8]) {
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_POINTS);
  printf("New iteration\n");
  for (size_t i = 0; i < 8; ++i) {
    glColor3f(i / 8.0, (i % 4) / 4.0, (i % 2) / 2.0);
    glVertex2i((int)cords[i].x, (int)cords[i].y);
    glVertex2i((int)cords[i].x + 1, (int)cords[i].y + 1);
    int next = (i + 1) % 8;
    draw_line(cords[i], cords[next]);
    printf("(%f, %f)\n", cords[i].x, cords[i].y);
  }
  draw_line(cords[0], cords[7]);
  draw_line(cords[0], cords[3]);
  draw_line(cords[1], cords[6]);
  draw_line(cords[2], cords[5]);
  draw_line(cords[4], cords[7]);
  glEnd();
}
int main() {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  GLFWwindow *window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cube", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Failed to open window\n");
    return -1;
  }

  Cube cube = init_cube(WINDOW_WIDTH / 2.0f - 100, WINDOW_HEIGHT / 2.0f - 100, 0, 200);
  Cord *cords = (Cord *)calloc(8, sizeof(Cord));
  glfwMakeContextCurrent(window);
  glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
  float i = 0;

  while (!glfwWindowShouldClose(window)) {
    rotate_cube(&cube, 0.1 * sin(i), 0.1 * cos(i), 0.1 * sin(i) * cos(i));
    project_cube(&cube, cords, ORTHOGRAPHIC);
    draw(cords);

    glfwSwapBuffers(window);
    glfwPollEvents();
    i += 0.01;
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  free(cords);
  return 0;
}
