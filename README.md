#### 3d snake game

https://youtu.be/efx-MN4MkO0

Build (lGL, lGLU, lGLEW, lglut needed):

`$ make`

Run:

`$ ./snakegl`

Controls:
```
,     - turn CW
.     - turn CCW
SPACE - pause
```

P.S. multisampling was added on recent commit, which is heavily dependent on your GPU. If you run the game and end up with "all-black" instead of any rendering, just erase out the keyword "sample" in fragment shader source file "snakegl.frag" (no need to recompile anything).
