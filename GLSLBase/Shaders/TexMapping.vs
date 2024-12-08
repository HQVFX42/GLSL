#version 450

in vec4 a_Position;
in vec2 a_Tex;

out vec2 v_Tex;

void main()
{
	gl_Position = vec4(a_Position.xyz, 1);
	v_Tex = a_Tex;
}
