#version 450

layout(location=0) out vec4 FragColor;

in vec2 v_Tex;

uniform float u_Time;

void main()
{
	vec4 newColour = vec4(0);
	
	//Q1. Sin모양 출력해보기 v_Tex : x = 0~1 --> 0~2PI
	/*
	float x = v_Tex.x * 2 * 3.141592;
	float sinResult = (sin(x) + 1) * 0.5; 
	if(sinResult > v_Tex.y)
	{
		newColour = vec4(1);	
	}
	*/
	//Q2. Sin모양 선만 출력해보기
	/*
	float x = v_Tex.x * 2 * 3.141592;
	float sinResult = (sin(x) + 1) * 0.5; 
	if(sinResult > v_Tex.y && sinResult < v_Tex.y+0.01)
	{
		newColour = vec4(1);	
	}
	*/
	//Q3. Sin 모양 움직이기
	float a = u_Time;
	float x = v_Tex.x * 2 * 3.141592;
	float sinResult = (sin(x + a) + 1) * 0.5; 
	if(sinResult > v_Tex.y && sinResult < v_Tex.y+0.01)
	{
		newColour = vec4(sin(u_Time)+1, cos(u_Time)+1, sin(u_Time)+2, 1);	
	}
	FragColor = newColour;
}