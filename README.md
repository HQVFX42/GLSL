# GLSL

## CPU Convolution Filtering
* TexMapping.fs
```cpp
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
* TexMapping.vs
```cpp
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
```cpp
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
```cpp
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
* ComputeTest.comp
```cpp
#version 450

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba8, binding = 0) uniform image2D inputImage;
layout (rgba8, binding = 1) uniform image2D outputImage;

uniform float kernel[9]; // 3x3 convolution kernel

void main() {
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    
    vec4 result = vec4(0.0);
    
    for (int j = -1; j <= 1; ++j) {
        for (int i = -1; i <= 1; ++i) {
            ivec2 offset = ivec2(i, j);
            vec4 pixel = imageLoad(inputImage, pixelCoords + offset);
            result += pixel * kernel[(j+1)*3 + (i+1)];
        }
    }
    
    result.a = imageLoad(inputImage, pixelCoords).a;
    
    result = clamp(result, 0.0, 1.0);
    
    imageStore(outputImage, pixelCoords, result);
}
```
* Method
* Kernels
