#version 450

in vec4 a_Position;
in vec2 a_UV;

out vec4 v_Temp;

void main()
{
	gl_Position = vec4(a_Position.xyz, 1);

	v_Temp = vec4(a_UV.xy, 0, 1);
}
