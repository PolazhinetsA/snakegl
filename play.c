#include "snakegl.h"

extern char field[N][M];
extern struct snake_t snake;

void draw_func()
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for (int i = 0; i < N * M; ++i) {
    if (((char*) field)[i])
      glDrawElementsInstancedBaseVertex (GL_TRIANGLE_STRIP, 15, GL_UNSIGNED_BYTE, (void*)5, ((char*)field)[i] == Fo ? 1 : 2, i*8);
    else
      glDrawElementsBaseVertex (GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (void*)0, i*8);
  }
  glFlush();
}

GLboolean zpause = GL_FALSE;

void move_func (int dir)
{
  static GLuint delay = 500,
                score = 0;
  if (zpause) {
    glutTimerFunc (delay, move_func, 0);
    return;
  }

  dir = snake.dir;
  int dx, dy;

  dx = dy = 0;
  switch (dir) {
    case Ri: dx =  1; break;
    case Le: dx = -1; break;
    case Up: dy =  1; break;
    case Do: dy = -1; break;
  }
  field[snake.hy][snake.hx] = dir;
  snake.hx += dx;
  snake.hy += dy;

  if ( N <= (unsigned) snake.hy
    || M <= (unsigned) snake.hx
    || field[snake.hy][snake.hx]
    && field[snake.hy][snake.hx] != Fo)
  {
    printf ("%u\n", score);
    exit (0);
  }

  if (field[snake.hy][snake.hx] == Fo) {
    int fx, fy;
    do fx = rand() % M, fy = rand() % N;
    while (field[fy][fx]);
    field[fy][fx] = Fo;
    delay -= delay/40;
    ++score;
  } else {
    dx = dy = 0;
    switch (field[snake.ty][snake.tx]) {
      case Ri: dx =  1; break;
      case Le: dx = -1; break;
      case Up: dy =  1; break;
      case Do: dy = -1; break;
    }
    field[snake.ty][snake.tx] = No;
    snake.tx += dx;
    snake.ty += dy;
  }
  field[snake.hy][snake.hx] = dir;

  glutTimerFunc (delay, move_func, 0);
}

void rot_func (int val)
{
  static float rotm1[3][3] =
  { { 1, 0, 0 },
    { 0, 1, 0 },
    { 0, 0, 1 } };
  static float ang = 0;

  rotm1[0][0] =  cos (ang);
  rotm1[0][1] =  sin (ang);
  rotm1[1][0] = -sin (ang);
  rotm1[1][1] =  cos (ang);

  glUniformMatrix3fv (unif_loc[UNIF_ROTM1], 1, GL_FALSE, rotm1);
  
  ang += 0.01;
  glutTimerFunc (50, rot_func, 0);
}

void keyb_func (unsigned char key, int x, int y)
{
  switch (key) {
    case ',': snake.dir = 1 + (snake.dir  ) % 4; break;
    case '.': snake.dir = 1 + (snake.dir+2) % 4; break;
    case ' ': zpause = !zpause; break;
  }
}
