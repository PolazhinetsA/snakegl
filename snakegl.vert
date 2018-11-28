#version 400 core

layout (location = 0) in vec4 in_pos;
layout (location = 1) in vec4 in_col;

out vec4 ex_col;

uniform mat3 rotm1;
uniform mat3 rotm2;
uniform vec3 movv;

void main()
{
  vec4 pos = in_pos;
  pos.z += 0.01 * gl_InstanceID;
  pos.xyz = pos.xyz * rotm1 * rotm2 + movv;
  gl_Position = vec4 (pos.xy / -pos.z, pos.zw);
  ex_col = in_col;
}
