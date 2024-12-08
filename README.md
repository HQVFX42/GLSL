# GLSL

## CPU Convolution Filtering
* Shaders
``` TexMaaping.fs
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
```
``` TexMaaping.vs
#version 450

in vec4 a_Position;
in vec2 a_Tex;

out vec2 v_Tex;

void main()
{
	gl_Position = vec4(a_Position.xyz, 1);
	v_Tex = a_Tex;
}

```
* Method
```
Image Renderer::ApplyConvolution(const Image& src, const std::vector<float>& kernel, int kernelSize)
{
	Image dst;
	dst.width = src.width;
	dst.height = src.height;
	dst.data.resize(src.data.size());

	int channels = 4; // RGBA
	int halfKernel = kernelSize / 2;

	for (int y = halfKernel; y < src.height - halfKernel; ++y)
	{
		for (int x = halfKernel; x < src.width - halfKernel; ++x)
		{
			for (int c = 0; c < channels; ++c)
			{
				float sum = 0.0f;
				for (int ky = -halfKernel; ky <= halfKernel; ++ky)
				{
					for (int kx = -halfKernel; kx <= halfKernel; ++kx)
					{
						int idx = ((y + ky) * src.width + (x + kx)) * channels + c;
						sum += src.data[idx] * kernel[(ky + halfKernel) * kernelSize + (kx + halfKernel)];
					}
				}
				int idx = (y * src.width + x) * channels + c;
				dst.data[idx] = std::clamp(static_cast<int>(sum), 0, 255);
			}
			dst.data[(y * src.width + x) * channels + 3] = src.data[(y * src.width + x) * channels + 3];
		}
	}
	return dst;
}
```

* Kernels
```
Image Renderer::Sharpen(const Image& src)
{
	std::vector<float> kernel =
	{
		0, -1, 0,
		-1, 5, -1,
		0, -1, 0
	};
	return ApplyConvolution(src, kernel, 3);
}

Image Renderer::EdgeDetection(const Image& src)
{
	std::vector<float> kernel =
	{
		1, 0, -1,
		0, 0, 0,
		-1, 0, 1
	};
	return ApplyConvolution(src, kernel, 3);
}

Image Renderer::Blur(const Image& src)
{
	std::vector<float> kernel(9, 1.0f / 9.0f);
	return ApplyConvolution(src, kernel, 3);
}
```

## GPU Image Convolution Filtering
* Shaders
* Method
* Kernels
