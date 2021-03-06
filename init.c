#include "snakegl.h"

char field[N][M];
struct snake_t snake;

GLuint
  width, height,
  vao, vbo, ebo,
  fbuf, rbuf[RBUF_NUM],
  prog;

GLint unif_loc[UNIF_NUM];

void init()
{
  snake.tx = 0;
  snake.ty = 0;
  snake.hx = 4;
  snake.hy = 0;
  snake.dir = Ri;
  memset (field, Ri, 5);
  field[0][6] = Fo;

  struct { float x, y, z, r, g, b; } arr[N][M][8];

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < M; ++j) {
      arr[i][j][0].x = arr[i][j][4].x = (float) j / M / 2 - 0.25; 
      arr[i][j][0].y = arr[i][j][4].y = (float) i / N / 2 - 0.25; 
      arr[i][j][1].x = arr[i][j][5].x = arr[i][j][0].x + 0.5 / M;
      arr[i][j][1].y = arr[i][j][5].y = arr[i][j][0].y;
      arr[i][j][2].x = arr[i][j][6].x = arr[i][j][0].x + 0.5 / M;
      arr[i][j][2].y = arr[i][j][6].y = arr[i][j][0].y + 0.5 / N;
      arr[i][j][3].x = arr[i][j][7].x = arr[i][j][0].x;
      arr[i][j][3].y = arr[i][j][7].y = arr[i][j][0].y + 0.5 / N;
      arr[i][j][0].z = arr[i][j][1].z = arr[i][j][2].z = arr[i][j][3].z = 0;
      arr[i][j][4].z = arr[i][j][5].z = arr[i][j][6].z = arr[i][j][7].z = 0.01;

      arr[i][j][0].r = arr[i][j][0].g = arr[i][j][0].b = 0.4;
      arr[i][j][1].r = arr[i][j][1].g = arr[i][j][1].b = 0.7;
      arr[i][j][2].r = arr[i][j][2].g = arr[i][j][2].b = 0.4;
      arr[i][j][3].r = arr[i][j][3].g = arr[i][j][3].b = 0.1;
      arr[i][j][4].r = arr[i][j][5].r = arr[i][j][6].r = arr[i][j][7].r = 1.0;
      arr[i][j][4].g = arr[i][j][5].g = arr[i][j][6].g = arr[i][j][7].g = 0.7;
      arr[i][j][4].b = arr[i][j][5].b = arr[i][j][6].b = arr[i][j][7].b = 0.4;
    }
  }

  GLubyte idx[] =
  { 0, 1, 3, 2, -1,
    0, 4, 1, 5, 2, 6, 3, 7, 0, 4, -1,
    4, 5, 7, 6, -1 };

  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);

  glGenBuffers (1, &vbo);
  glBindBuffer (GL_ARRAY_BUFFER, vbo);
  glBufferData (GL_ARRAY_BUFFER, sizeof (arr), arr, GL_STATIC_DRAW);

  glGenBuffers (1, &ebo);
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (idx), idx, GL_STATIC_DRAW);

  glEnable (GL_PRIMITIVE_RESTART);
  glPrimitiveRestartIndex (255);

  prog = load_shaders ("snakegl.vert", "snakegl.frag");
  glUseProgram (prog);

  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, sizeof (arr[0][0][0]), (void*) 0);
  glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, sizeof (arr[0][0][0]), (void*) (3 * sizeof (float)));
  glEnableVertexAttribArray (0);
  glEnableVertexAttribArray (1);

  unif_loc[UNIF_ROTM1] = glGetUniformLocation (prog, "rotm1");
  unif_loc[UNIF_ROTM2] = glGetUniformLocation (prog, "rotm2");
  unif_loc[UNIF_MOVV]  = glGetUniformLocation (prog, "movv" );
  unif_loc[UNIF_ARF]   = glGetUniformLocation (prog, "arf"  );
  unif_loc[UNIF_TOPB]  = glGetUniformLocation (prog, "topb" );

  float
    rotm2[3][3] =
    { { 1, 0, 0 },
      { 0, 1, 0 },
      { 0, 0, 1 } },
    movv[3] =
    { 0, 0, -0.5 };

  rotm2[1][1] =  cos (1.0);
  rotm2[1][2] =  sin (1.0);
  rotm2[2][1] = -sin (1.0);
  rotm2[2][2] =  cos (1.0);

  glUniformMatrix3fv (unif_loc[UNIF_ROTM2], 1, GL_FALSE, rotm2);
  glUniform3fv (unif_loc[UNIF_MOVV], 1, movv);

  glGenRenderbuffers (RBUF_NUM, rbuf);
  glBindRenderbuffer (GL_RENDERBUFFER, rbuf[RBUF_COL]);
  glRenderbufferStorage (GL_RENDERBUFFER, GL_RGBA, 512, 512);
  glBindRenderbuffer (GL_RENDERBUFFER, rbuf[RBUF_DEP]);
  glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

  glGenFramebuffers (1, &fbuf);
  glBindFramebuffer (GL_DRAW_FRAMEBUFFER, fbuf);
  glFramebufferRenderbuffer (GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_RENDERBUFFER, rbuf[RBUF_COL]);
  glFramebufferRenderbuffer (GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                             GL_RENDERBUFFER, rbuf[RBUF_DEP]);

  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_GREATER);
  glClearDepth (-1.0);
}

GLuint load_shaders(char *vpath, char *fpath)
{
  FILE *file;
  int sz;

  file = fopen (vpath, "r");
  fseek (file, 0, SEEK_END);
  sz = ftell (file);
  rewind (file);
  char buf1[sz+1], *pbuf1 = buf1;
  fread (buf1, 1, sz, file);
  buf1[sz] = '\0';
  fclose (file);

  file = fopen (fpath, "r");
  fseek (file, 0, SEEK_END);
  sz = ftell (file);
  rewind (file);
  char buf2[sz+1], *pbuf2 = buf2;
  fread (buf2, 1, sz, file);
  buf2[sz] = '\0';
  fclose (file);

  int vert_shader = glCreateShader (GL_VERTEX_SHADER),
      frag_shader = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource (vert_shader, 1, &pbuf1, NULL);
  glShaderSource (frag_shader, 1, &pbuf2, NULL);
  glCompileShader (vert_shader);
  glCompileShader (frag_shader);

  GLboolean status1, status2;
  glGetShaderiv (vert_shader, GL_COMPILE_STATUS, &status1);
  glGetShaderiv (frag_shader, GL_COMPILE_STATUS, &status2);

  if (!status1 || !status2) {
    GLsizei sz, len1, len2;
    glGetShaderiv (vert_shader, GL_INFO_LOG_LENGTH, &len1);
    glGetShaderiv (frag_shader, GL_INFO_LOG_LENGTH, &len2);
    sz = (len1 > len2 ? len1 : len2) + 1;
    char buf[sz];
    glGetShaderInfoLog (vert_shader, sz, NULL, buf);
    puts (buf);
    glGetShaderInfoLog (frag_shader, sz, NULL, buf);
    puts (buf);
    exit (1);
  }

  int prog = glCreateProgram();
  glAttachShader (prog, vert_shader);
  glAttachShader (prog, frag_shader);
  glLinkProgram (prog);

  return prog;
}
