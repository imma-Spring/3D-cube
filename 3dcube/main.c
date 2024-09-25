#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

#include "cube.h"

#define MAX_INTERSECTIONS 1000

float current_color = 0;
Projection p = ORTHOGRAPHIC;

Cord intersections[MAX_INTERSECTIONS];
int intersection_count = 0;

static Cord centroid;

float sqr(float x) { return x * x; }

float distance(float x1, float y1, float x2, float y2) {
  return sqrt(sqr(x1 - x2) + sqr(y1 - y2));
}

float max(float a, float b) { return a < b ? b : a; }
float true_abs(float x) { return x < 0 ? -x : x; }

void add_intersection(float x, float y) {
  if (intersection_count < MAX_INTERSECTIONS) {
    Cord intersection = {x, y, 0, 0, 0};
    intersections[intersection_count++] = intersection;
  }
}

Cord calculate_centroid() {
  Cord centroid = {0, 0, 0, 0, 0};
  for (int i = 0; i < intersection_count; ++i) {
    centroid.x += intersections[i].x;
    centroid.y += intersections[i].y;
  }
  centroid.x /= intersection_count;
  centroid.y /= intersection_count;
  return centroid;
}

int compare_angle(const void *a, const void *b) {
  Cord *p1 = (Cord *)a;
  Cord *p2 = (Cord *)b;
  float angle1 = atan2(p1->y - centroid.y, p1->x - centroid.x);
  float angle2 = atan2(p2->y - centroid.y, p2->x - centroid.x);

  return (angle1 < angle2) ? -1 : (angle1 > angle2) ? 1 : 0;
}

void sort_intersections() {
  qsort(intersections, intersection_count, sizeof(Cord), compare_angle);
}

int compare_depth(const Cord *a, const Cord *b) {
  Cord *c1 = (Cord *)a;
  Cord *c2 = (Cord *)b;
  if (c1->tdz < c2->tdz)
    return 1;
  if (c1->tdz > c2->tdz)
    return -1;
  return 0;
}

void sort_cords_by_depth(Cord *cords, int *order, int count) {
  for (int i = 0; i < count - 1; ++i) {
    for (int j = i + 1; j < count; ++j) {
      int v = compare_depth(&cords[i], &cords[j]);
      if (v == 1) {
        Cord t = cords[i];
        cords[i] = cords[j];
        cords[j] = t;
        int p = order[i];
        order[i] = order[j];
        order[j] = p;
      }
    }
  }
}

void draw_line(Cord c1, Cord c2) {
  if (c1.tdz > 0 && c2.tdz > 0)
    return;
  if (c1.tdz > 0 || c2.tdz > 0) {
    Cord *behind = c1.tdz > 0 ? &c1 : &c2;
    Cord *front = c1.tdz <= 0 ? &c1 : &c2;
    float my = (behind->tdy - front->tdy) / (behind->tdz - front->tdz);
    float mx = (behind->tdx - front->tdx) / (behind->tdz - front->tdz);
    float ix = -mx * front->tdz + front->tdx;
    float iy = -my * front->tdz + front->tdy;
    float iz = 0;
    Cord intersection;
    if (p == PERSPECTIVE) {
      intersection = getPerspective(ix, iy, iy);
    } else {
      Cord c = {ix, iy, ix, iy, iz};
      intersection = c;
    }

    behind->x = intersection.x;
    behind->y = intersection.y;
    behind->tdx = ix;
    behind->tdy = iy;
    behind->tdz = iz;

    add_intersection(intersection.x, intersection.y);
  }

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
    float p = 1;
    float inc = (y1 - yf) / (x1 - xf) * p;
    for (float x = x1, y = y1; x <= xf; x += p, y += inc) {
      int tx = (int)x;
      int ty = (int)y;
      float dist = distance(x, y, tx, ty);
      float opacity = max(0, 1 - dist);
      glColor4f((500 - c1.tdz) / 500, (500 - c1.tdz) / 500,
                (500 - c1.tdz) / 500, 0.3);
      glVertex2i(tx, ty);
      dist = distance(x, y + 1, tx, ty + 1);
      opacity = max(0, 1 - dist);
      glColor4f(current_color, current_color + 0.001, current_color + 0.002, 0.3);
      current_color += 0.002;
      current_color *= current_color > 1 ? 0 : 1;
      glVertex2i(tx, ty + 1);
      /*dist = distance(x, y - 1, tx, ty - 1);
      opacity = max(0, 1 - dist);
      glColor3f(opacity, opacity, opacity);
      glVertex2i(tx, ty - 1);*/
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
    float p = 1;
    float inc = (x1 - xf) / (y1 - yf) * p;
    for (float y = y1, x = x1; y <= yf; y += p, x += inc) {
      int tx = (int)x;
      int ty = (int)y;
      float dist = distance(x, y, tx, ty);
      float opacity = max(0, 1 - dist);
      glColor4f(1, 1, 1, 0.3);
      glVertex2i(tx, ty);
      dist = distance(x + 1, y, tx + 1, ty);
      opacity = max(0, 1 - dist);
      glColor4f((500 - c1.tdz) / 500, (500 - c1.tdz) / 500,
                (500 - c1.tdz) / 500, 0.3);
      current_color += 0.0000001;
      current_color *= current_color > 1 ? 0 : 1;
      glVertex2i(tx + 1, ty);
      /*dist = distance(x - 1, y, tx - 1, ty);
      opacity = max(0, 1 - dist);
      glColor3f(opacity, opacity, opacity);
      glVertex2i(tx - 1, ty);*/
    }
  }
}

void draw_filled_polygon() {
  if (intersection_count < 3) {
    return;
  }
  centroid = calculate_centroid();
  sort_intersections();

  glColor4f(0.5, 0.5, 0.5, 0.01);
  glBegin(GL_POLYGON);
  for (int i = 0; i < intersection_count; ++i) {
    glVertex2f(intersections[i].x, intersections[i].y);
  }
  glEnd();
}

static bool contains(int *arr, int a, int length) {
  for (int i = 0; i < length; ++i) {
    if (arr[i] == a)
      return true;
  }
  return false;
}

void draw(Cord cords[8]) {
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_POINTS);
  printf("New iteration\n");
  intersection_count = 0;
  int order[8];
  for (int i = 0; i < 8; ++i) {
    order[i] = i;
  }
  sort_cords_by_depth(cords, order, 8);
  for (size_t i = 0; i < 8; ++i) {
    glColor3f(current_color, current_color + 0.01, current_color + 0.03);
    current_color += 0.0000001;
    current_color *= current_color > 1 ? 0 : 1;
    if (cords[i].tdz <= 0) {
      glVertex2i((int)cords[i].x, (int)cords[i].y);
      glVertex2i((int)cords[i].x + 1, (int)cords[i].y + 1);
    }
    printf("(%f, %f)\n", cords[i].x, cords[i].y);
  }

  bool visited[8] = {false, false, false, false, false, false, false, false};
  for (int i = 0; i < 8; ++i) {
    Cord c = cords[i];
    int index = order[i];
    visited[index] = true;
    for (int j = 0; j < 8; ++j) {
      if (j == i)
        continue;
      int index2 = order[j];
      if (!visited[index2]) {
        switch (index) {
        case 0: {
          if (index2 == 1 || index2 == 3 || index2 == 7)
            draw_line(c, cords[j]);
          break;
        }
        case 1: {
          if (index2 == 0 || index2 == 2 || index2 == 6)
            draw_line(c, cords[j]);
          break;
        }
        case 2: {
          if (index2 == 1 || index2 == 3 || index2 == 5)
            draw_line(c, cords[j]);
          break;
        }
        case 3: {
          if (index2 == 0 || index2 == 2 || index2 == 4)
            draw_line(c, cords[j]);
          break;
        }
        case 4: {
          if (index2 == 5 || index2 == 7 || index2 == 3)
            draw_line(c, cords[j]);
          break;
        }
        case 5: {
          if (index2 == 4 || index2 == 6 || index2 == 2)
            draw_line(c, cords[j]);
          break;
        }
        case 6: {
          if (index2 == 5 || index2 == 7 || index2 == 1)
            draw_line(c, cords[j]);
          break;
        }
        case 7: {
          if (index2 == 4 || index2 == 6 || index2 == 0)
            draw_line(c, cords[j]);
          break;
        }
        }
      }
    }
  }

  printf("%f\n", current_color);
  glEnd();
  draw_filled_polygon();
  intersection_count = 0;
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

  Cube cube = init_cube(WINDOW_WIDTH / 2.0f - 100, WINDOW_HEIGHT / 2.0f - 100,
                        -200, 200);
  Cord *cords = (Cord *)calloc(8, sizeof(Cord));
  glfwMakeContextCurrent(window);
  glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
  float i = 0;
  float j = 0;

  while (!glfwWindowShouldClose(window)) {
    rotate_cube(&cube, 0.01 * sin(i), 0.01 * cos(i), 0.01 * sin(i) * cos(i));
    move_cube(&cube, sin(j), cos(j), sin(j) * cos(j));
    project_cube(&cube, cords, p);
    draw(cords);

    glfwSwapBuffers(window);
    glfwPollEvents();
    i += 0.01;
    j += 0.1;
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  free(cords);
  return 0;
}
