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
	* Original texture
![스크린샷 2024-12-08 234732](https://github.com/user-attachments/assets/d8536238-61ad-4913-8f05-15239e31d28a)

	* Sharpening
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
```
![스크린샷 2024-12-08 234708](https://github.com/user-attachments/assets/f0ea75de-07bd-409f-8c82-1a492edb6fa3)

	* Edge detection
```cpp
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
```
![스크린샷 2024-12-08 234516](https://github.com/user-attachments/assets/2dcfd9a2-c738-47e1-8412-34ac24c530fc)

	* Blur
```cpp
Image Renderer::Blur(const Image& src)
{
	std::vector<float> kernel(9, 1.0f / 9.0f);
	return ApplyConvolution(src, kernel, 3);
}
```
![스크린샷 2024-12-08 234631](https://github.com/user-attachments/assets/c7317954-2832-4424-beb0-314a0bf44ec5)


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
```cpp
void Renderer::GPUConvolutionFilter()
{
	std::string comp;
	ReadFile("./Shaders/ComputeTest.comp", &comp);
	const char* computeShaderSource = comp.c_str();
	m_ComputeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(m_ComputeShader, 1, &computeShaderSource, NULL);
	glCompileShader(m_ComputeShader);

	m_ComputeProgram = glCreateProgram();
	glAttachShader(m_ComputeProgram, m_ComputeShader);
	glLinkProgram(m_ComputeProgram);

	Image img;
	glBindTexture(GL_TEXTURE_2D, m_TextureParameter2D);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&img.width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&img.height);

	glGenTextures(1, &m_OutputTexture);
	glBindTexture(GL_TEXTURE_2D, m_OutputTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, img.width, img.height);

	float kernel[9];
	switch (2)
	{
	case 0: // Sharpen
		kernel[0] = -1; kernel[1] = -1; kernel[2] = -1;
		kernel[3] = -1; kernel[4] = 9; kernel[5] = -1;
		kernel[6] = -1; kernel[7] = -1; kernel[8] = -1;
		break;
	case 1: // Edge Detection
		kernel[0] = -1; kernel[1] = -1; kernel[2] = -1;
		kernel[3] = -1; kernel[4] = 8; kernel[5] = -1;
		kernel[6] = -1; kernel[7] = -1; kernel[8] = -1;
		break;
	case 2:	// Embossing
		kernel[0] = 2; kernel[1] = -0; kernel[2] = 0;
		kernel[3] = 0; kernel[4] = -1; kernel[5] = 0;
		kernel[6] = 0; kernel[7] = 0; kernel[8] = -1;
	default:
		break;
	}

	glUseProgram(m_ComputeProgram);
	glUniform1fv(glGetUniformLocation(m_ComputeProgram, "kernel"), 9, kernel);

	glBindImageTexture(0, m_TextureParameter2D, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_OutputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glDispatchCompute(img.width / 16, img.height / 16, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glCopyImageSubData(m_OutputTexture, GL_TEXTURE_2D, 0, 0, 0, 0,
		m_TextureParameter2D, GL_TEXTURE_2D, 0, 0, 0, 0,
		img.width, img.height, 1);
}
```
* Kernels
```cpp
float kernel[9];
	switch (2)
	{
	case 0: // Sharpen
		kernel[0] = -1; kernel[1] = -1; kernel[2] = -1;
		kernel[3] = -1; kernel[4] = 9; kernel[5] = -1;
		kernel[6] = -1; kernel[7] = -1; kernel[8] = -1;
		break;
	case 1: // Edge Detection
		kernel[0] = -1; kernel[1] = -1; kernel[2] = -1;
		kernel[3] = -1; kernel[4] = 8; kernel[5] = -1;
		kernel[6] = -1; kernel[7] = -1; kernel[8] = -1;
		break;
	case 2:	// Embossing
		kernel[0] = 2; kernel[1] = -0; kernel[2] = 0;
		kernel[3] = 0; kernel[4] = -1; kernel[5] = 0;
		kernel[6] = 0; kernel[7] = 0; kernel[8] = -1;
	default:
		break;
	}
```
	* Sharpen
![스크린샷 2024-12-08 234259](https://github.com/user-attachments/assets/6cbaa5f9-6d74-4457-8dc3-fe41449142ad)

	* Edge Detection
![스크린샷 2024-12-08 234403](https://github.com/user-attachments/assets/9880a874-da23-430b-8c6e-821ab898c19c)

	* Embossing
![스크린샷 2024-12-08 234416](https://github.com/user-attachments/assets/fc06ad07-14d7-4e1c-bafd-4839eeecc8c6)



## Render Scene
* Load shader code
```cpp
bool Renderer::ReadFile(char* filename, std::string* target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}
```
* Compile shaders
```cpp
void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = (GLint)strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.\n";

	return ShaderProgram;
}
```

* Load Pngs: Load file and decode image.
```cpp
m_TextureParameter2D = CreatePngTexture("./Textures/Banana.png");

GLuint Renderer::CreatePngTexture(char* filePath)
{
	//Load Pngs: Load file and decode image.
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filePath);
	if (error != 0)
	{
		lodepng_error_text(error);
		assert(error == 0);
		return -1;
	}

	GLuint temp;
	glGenTextures(1, &temp);

	glBindTexture(GL_TEXTURE_2D, temp);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	return temp;
}
```
* Main method
```cpp
// Initialize Renderer
g_Renderer = new Renderer(g_WindowSizeX, g_WindowSizeY);

//g_Renderer->ApplyFilterAndUpdateTexture();
g_Renderer->GPUConvolutionFilter();

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

g_Renderer->DrawRectTex(0);

glutSwapBuffers();

glutMainLoop();

delete g_Renderer;
```
