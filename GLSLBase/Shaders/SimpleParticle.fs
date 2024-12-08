#version 450

layout(location=0) out vec4 FragColor;

uniform sampler2D u_Texture;

in vec4 v_Colour;
in vec2 v_OriginXY;
in float v_Radius;
in vec2 v_TexPos;

void main()
{

	vec4 newColour;
	float dis = sqrt(v_OriginXY.x * v_OriginXY.x + v_OriginXY.y * v_OriginXY.y);
	if(dis < v_Radius)
	{
		newColour = v_Colour;
		newColour.a =pow(dis / v_Radius, 2);	//�� ���
	}
	else
	{
		newColour = vec4(0);
	}

	FragColor = newColour;
	

	//FragColor = texture(u_Texture, v_TexPos);
}
