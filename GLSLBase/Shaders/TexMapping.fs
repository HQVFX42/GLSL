#version 450

layout(location=0) out vec4 FragColor;

in vec2 v_Tex;

uniform float u_Time;
uniform sampler2D u_Texture;

void main()
{
	vec2 newTex = abs(v_Tex - vec2(0.f, 1.0f)) + vec2(0.f, 1.0f);

	vec4 newColour = vec4(newTex, 0, 1);

	newColour = texture(u_Texture, newTex);

	FragColor = newColour;
}