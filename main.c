#include "snakegl.h"

int main (int argc, char **argv)
{
  glutInit (&argc, argv);
  glutInitContextVersion (4, 0);
  glutInitContextProfile (GLUT_CORE_PROFILE);
  glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutInitWindowSize (640, 480);
  glutCreateWindow (argv[0]);

  glewExperimental = GL_TRUE;
  glewInit();

  init();

  glutDisplayFunc (draw_func);
  glutTimerFunc (500, move_func, 0);
  glutTimerFunc (50, rot_func, 0);
  glutKeyboardFunc (keyb_func);

  glutMainLoop();
}
