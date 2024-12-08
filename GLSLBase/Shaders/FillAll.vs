#version 450

in vec4 a_Position;

void main()
{
	gl_Position = vec4(a_Position);
}
