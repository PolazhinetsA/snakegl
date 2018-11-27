#include "snakegl.h"

char field[N][M];
struct snake_t snake;

GLuint vao_id,
       vbo_id,
       prog_id;

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

  glGenVertexArrays (1, &vao_id);
  glBindVertexArray (vao_id);

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

  glGenBuffers (1, &vbo_id);
  glBindBuffer (GL_ARRAY_BUFFER, vbo_id);
  glBufferData (GL_ARRAY_BUFFER, sizeof (arr), arr, GL_STATIC_DRAW);

  prog_id = load_shaders ("snakegl.vert", "snakegl.frag");
  glUseProgram (prog_id);

  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, sizeof (arr[0][0][0]), (void*) 0);
  glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, sizeof (arr[0][0][0]), (void*) (3 * sizeof (float)));
  glEnableVertexAttribArray (0);
  glEnableVertexAttribArray (1);

  unif_loc[UNIF_ROTM1] = glGetUniformLocation (prog_id, "rotm1");
  unif_loc[UNIF_ROTM2] = glGetUniformLocation (prog_id, "rotm2");
  unif_loc[UNIF_MOVV]  = glGetUniformLocation (prog_id, "movv");

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
  glUniform3fv       (unif_loc[UNIF_MOVV],  1,           movv);

  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_GREATER);
  glClearDepth (-1.0);
}

GLuint load_shaders(char *vpath, char *fpath)
{
  int fd1, fd2;
  struct stat st1, st2;
  fd1 = open (vpath, O_RDONLY);
  fd2 = open (fpath, O_RDONLY);
  fstat (fd1, &st1);
  fstat (fd2, &st2);
  char buf1[st1.st_size + 1],
       buf2[st2.st_size + 1],
       *pbuf1 = buf1,
       *pbuf2 = buf2;
  buf1[read (fd1, buf1, st1.st_size)] = '\0';
  buf2[read (fd2, buf2, st2.st_size)] = '\0';
  close (fd1);
  close (fd2);

  int vert_shader_id = glCreateShader (GL_VERTEX_SHADER),
      frag_shader_id = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource (vert_shader_id, 1, &pbuf1, NULL);
  glShaderSource (frag_shader_id, 1, &pbuf2, NULL);
  glCompileShader (vert_shader_id);
  glCompileShader (frag_shader_id);

  GLboolean status1, status2;
  glGetShaderiv (vert_shader_id, GL_COMPILE_STATUS, &status1);
  glGetShaderiv (frag_shader_id, GL_COMPILE_STATUS, &status2);

  if (! (status1 && status2)) {
    GLsizei sz, len1, len2;
    glGetShaderiv (vert_shader_id, GL_INFO_LOG_LENGTH, &len1);
    glGetShaderiv (frag_shader_id, GL_INFO_LOG_LENGTH, &len2);
    sz = (len1 > len2 ? len1 : len2) + 1;
    char buf[sz];
    glGetShaderInfoLog (vert_shader_id, sz, NULL, buf);
    puts (buf);
    glGetShaderInfoLog (frag_shader_id, sz, NULL, buf);
    puts (buf);
    exit (1);
  }

  int prog_id = glCreateProgram();
  glAttachShader (prog_id, vert_shader_id);
  glAttachShader (prog_id, frag_shader_id);
  glLinkProgram (prog_id);

  return prog_id;
}
