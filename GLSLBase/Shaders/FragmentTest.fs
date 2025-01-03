#version 450

layout(location=0) out vec4 FragColor;

in vec4 v_Temp;

uniform float u_Time;
uniform vec2 u_Points[5];

const float PI = 3.141592;

void main()
{
	vec2 newUV = v_Temp.xy - vec2(0.5f, 0.5f);		//-0.5~0.5, x, y

	float pointGrey = 0;

	float distance = length(newUV);
	float newTime = fract(u_Time);
	float ringWidth = 0.1;

	if(distance < newTime + ringWidth && distance > newTime - ringWidth)
	{
		float temp = (distance - newTime) / ringWidth;
		pointGrey = temp;

		for(int i = 0; i < 5; i++)
		{
			vec2 newPoint = u_Points[i];
			vec2 newVec = newPoint - newUV;
			float distance = sqrt(newVec.x * newVec.x + newVec.y * newVec.y); 
			if(distance < 0.1f)
			{
				pointGrey += 0.5f * pow((1-(distance / 0.1f)), 10);
			}
		}
	}

	FragColor = vec4(pointGrey);

	/*
	//내부가 빈 원 그리기
	distance = sqrt(newUV.x * newUV.x + newUV.y * newUV.y);
	float grey = sin(distance * PI * 2f * 5);
    float outerRadius = 0.5f;
    float innerRadius = 0.48f;
	if(distance < outerRadius && distance > innerRadius)
	{
		FragColor = vec4(1);	// 원의 테두리는 흰색
	}
	else
	{
		FragColor = vec4(0);	// 그 외의 영역은 검은색
	}
	*/
}
