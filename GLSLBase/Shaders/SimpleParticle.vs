#version 450

// 위치 = 초기위치 + 초기속도 * 시간
// 속도 = 초기속도 + 가속도 * 시간
// 위치 = 초기위치 + 초기속도 * 시간 + 중력가속도(1/2g*t^2)

in vec3 a_Position;
in vec3 a_Velocity;
in vec4 a_StartLifeRatioAmp;
in float a_RandValue;
in vec4 a_Colour;
//
in vec2 TexPos;

//Varying --> fragment shader input
out vec4 v_Colour;
out vec2 v_OriginXY;
out float v_Radius;
//
out vec2 v_TexPos;

uniform float u_Time;	//0.f ~

const float PI = 3.141592;
const mat3 c_RP = mat3(0, -1, 0, 1, 0, 0, 0, 0, 0);	//RotateParticleMatrix (cos, -sin, 0, sin, 0, 0, 0, 0, 0)
const vec3 c_Gravity = vec3(0, -5, 0);

void main()
{
	vec3 newPos = a_Position.xyz;
	v_OriginXY = a_Position.xy;		//사각형 대신 원 출력하기 위한 xyz 원래 위치 출력
	v_Radius = abs(a_Position.x);
	vec3 newVel = a_Velocity.xyz;

	float startTime = a_StartLifeRatioAmp.x;
	float lifeTime = a_StartLifeRatioAmp.y;
	float ratio = a_StartLifeRatioAmp.z;
	float amp = a_StartLifeRatioAmp.w;

	float newTime = u_Time - startTime;
	float newAlpha = 1.f;

	if(newTime > 0)
	{
		newPos.x += sin(a_RandValue * PI * 2);	//* fract(u_Time); 원 퍼져나감
		newPos.y += cos(a_RandValue * PI * 2);	//* fract(u_Time);
			
		newTime = mod(newTime, lifeTime);

		newVel = newVel + c_Gravity * newTime;	//위치

		//newPos = 0.8f * newPos  + a_Velocity * newTime + (0.5*c_Gravity * newTime * newTime);		//속도
		newPos = 0.8f * newPos + 0.5*c_Gravity * newTime * newTime;

		vec3 vSin = a_Velocity * c_RP;

		//newPos = newPos + vSin * sin(newTime * 2 * PI * ratio) * amp;	//sineTrail
		
		newAlpha = pow(1- newTime / lifeTime, 10);	//alpha blending

		//amp = amp * newTime * newTime;	//점점 커지게
		//newPos.x += sin(newTime);
		//newPos.y += sin(newTime * 2 * PI * ratio) * amp;	//sin(주기) * 폭
		
	}
	else
	{
		newPos = vec3(10000, 10000, 10000);
	}
	
	gl_Position = vec4(newPos, 1);

	v_Colour = a_Colour;
	v_Colour.a = newAlpha;
	v_TexPos = TexPos;
}