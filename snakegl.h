#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define N 16
#define M 16

enum {No, Ri, Do, Le, Up, Fo};

enum {UNIF_ROTM1, UNIF_ROTM2, UNIF_MOVV, UNIF_ARF, UNIF_TOPB, UNIF_NUM};

enum {RBUF_COL, RBUF_DEP, RBUF_NUM};

struct snake_t {
  int hx, hy, tx, ty, dir;
};

extern char field[N][M];
extern struct snake_t snake;

extern GLuint
  width, height,
  vao, vbo, ebo,
  fbuf, rbuf[RBUF_NUM],
  prog;

extern GLint unif_loc[UNIF_NUM];

void init();
GLuint load_shaders (char*, char*);

void draw_func();
void move_func (int);
void rot_func (int);
void keyb_func (unsigned char, int, int);
void resz_func (int, int);
