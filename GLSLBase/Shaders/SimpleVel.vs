#version 450

in vec3 a_Position;
in vec3 a_Vel;
in vec2 a_StartLife;

// 위치 = 초기위치 + 초기속도 * 시간
// 속도 = 초기속도 + 가속도 * 시간
// 위치 = 초기위치 + 초기속도 * 시간 + 중력가속도(1/2g*t^2)

uniform float u_Time;	//0.f ~
uniform bool u_Repeat = true;

const vec3 c_Gravity = vec3(0, -5, 0);

void main()
{
	vec3 newPos = a_Position;
	float startTime = a_StartLife.x;
	float lifeTime = a_StartLife.y;

	//float newTime = fract(u_Time);	//0~1반복
	float newTime = u_Time - startTime;

	if(newTime > 0)
	{
		float life = newTime;

		float remainingLife = lifeTime - life;

		if(u_Repeat == true)
		{
			remainingLife = 1.f;
			newTime = mod(newTime, lifeTime);
		}

		if(remainingLife < 0)
		{
			newPos = vec3(10000,10000,10000);
		}
		else
		{
			newPos += a_Vel * newTime + (0.5 * c_Gravity) * (newTime * newTime);
		}
	}
	else
	{
		newPos = vec3(10000,10000,10000);
	}

	gl_Position = vec4(newPos, 1);
}