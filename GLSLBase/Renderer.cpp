#include "stdafx.h"
#include "Renderer.h"
#include "LoadPng.h"
#include <Windows.h>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <algorithm>

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}

Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_SimpleVelShader = CompileShaders("./Shaders/SimpleVel.vs", "./Shaders/SimpleVel.fs");
	m_SimpleParticleShader = CompileShaders("./Shaders/SimpleParticle.vs", "./Shaders/SimpleParticle.fs");
	m_FragmentTestShader = CompileShaders("./Shaders/FragmentTest.vs", "./Shaders/FragmentTest.fs");
	m_FillAllShader = CompileShaders("./Shaders/FillAll.vs", "./Shaders/FillAll.fs");
	m_RectTexShader = CompileShaders("./Shaders/RectTex.vs", "./Shaders/RectTex.fs");
	m_TexMappingShader = CompileShaders("./Shaders/TexMapping.vs", "./Shaders/TexMapping.fs");
	m_ComputeShader =

	//Load Textures
	m_ParticleTexture = CreatePngTexture("./Textures/Snowflakes.png");
	m_TextureParameter2D = CreatePngTexture("./Textures/Banana.png");
	m_BackGroundTexture = CreatePngTexture("./Textures/Input.png");

	//Create VBOs
	CreateVertexBufferObjects();

	//Create Textures
	CreateTextures();
	CreateGridMesh();
}

void Renderer::CreateVertexBufferObjects()
{
	float size = 1.f;
	float rect[]
		=
	{
		-size, -size, 0.f, 0.5f, 0,0,		//x, y, z, value, U,V
		-size, size, 0.f, 0.5f, 0,1,
		size, size, 0.f, 0.5f,	1,1,			//Triangle1
		-size, -size, 0.f, 0.5f, 0,0,
		size, size, 0.f, 0.5f, 1,1,
		size, -size, 0.f, 0.5f, 1,0			//Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	size = 0.5f;
	float texRect[]
		=
	{
			-size, -size, 0.f, 0,0,		//x, y, z, U,V
		-size, size, 0.f, 0,1,
		size, size, 0.f, 1,1,			//Triangle1
		-size, -size, 0.f, 0,0,
		size, size, 0.f, 1,1,
		size, -size, 0.f, 1,0			//Triangle2
	};
	glGenBuffers(1, &m_VBORectTex);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORectTex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texRect), texRect, GL_STATIC_DRAW);

	float colour[]
		=
	{
		-0.5, -0.5, 0.f, 0.5f,	//r, g, b, a
		-0.5, 0.5, 0.f, 0.5f,
		0.5, 0.5, 0.f, 0.5f,
		-0.5, -0.5, 0.f, 0.5f,
		0.5, 0.5, 0.f, 0.5f,
		0.5, -0.5, 0.f, 0.5f
	};

	glGenBuffers(1, &m_VBORectColour);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORectColour);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colour), colour, GL_STATIC_DRAW);

	//Lecture02
	float triangles[]
		=
	{
		-0.5f,0.f,0.f, 0.5f,0.f,0.f, 0.f,0.5f,0.f,	//T1
		-0.5f,0.25f,0.f, 0.5f,0.25f,0.f, 0.f,-0.25f,0.f	//T2
	};

	glGenBuffers(1, &mVBOTriangles);
	glBindBuffer(GL_ARRAY_BUFFER, mVBOTriangles);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

	GenQuadsVBO(1000, false, &mVBOQuads, &mVBOQuadsVertexCount);
	CreateGridMesh();
}

void Renderer::CreateTextures()
{
	static const GLulong checkerboard[] =
	{
		0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
		0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
		0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
		0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
		0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
		0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
		0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
		0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
	};

	glGenTextures(1, &m_checkerboardTexture);
	glBindTexture(GL_TEXTURE_2D, m_checkerboardTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerboard);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::GenQuads(int num)
{
	float size = 0.01f;
	int countQuad = num;
	int verticesPerQuad = 6;
	int floatsPerVertex = 3 + 3 + 2;
	int arraySize = countQuad * verticesPerQuad * floatsPerVertex;
	float* vertices = new float[arraySize];

	for (int i = 0; i < countQuad; i++)
	{
		float randX, randY, randZ;
		float randVelX, randVelY, randVelZ;
		float startTime, lifeTime;
		float startTimeMax = 10.f;
		float lifeTimeMax = 3.f;

		int index = i * verticesPerQuad * floatsPerVertex;

		randX = 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
		randY = 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
		randZ = 0.f;

		randVelX = 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
		randVelY = 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
		randVelZ = 0.f;

		startTime = (((float)rand() / (float)RAND_MAX) - 0.5f) * startTimeMax;
		lifeTime = (((float)rand() / (float)RAND_MAX) - 0.5f) * lifeTimeMax;

		vertices[index] = randX - size; index++;
		vertices[index] = randY - size; index++;
		vertices[index] = randZ; index++;
		vertices[index] = randVelX; index++;
		vertices[index] = randVelY; index++;
		vertices[index] = randVelZ; index++;
		vertices[index] = startTime; index++;
		vertices[index] = lifeTime; index++;

		vertices[index] = randX - size; index++;
		vertices[index] = randY + size; index++;
		vertices[index] = randZ; index++;
		vertices[index] = randVelX; index++;
		vertices[index] = randVelY; index++;
		vertices[index] = randVelZ; index++;
		vertices[index] = startTime; index++;
		vertices[index] = lifeTime; index++;

		vertices[index] = randX + size; index++;
		vertices[index] = randY + size; index++;
		vertices[index] = randZ; index++;
		vertices[index] = randVelX; index++;
		vertices[index] = randVelY; index++;
		vertices[index] = randVelZ; index++;
		vertices[index] = startTime; index++;
		vertices[index] = lifeTime; index++;
		//-------------------------------------------//
		vertices[index] = randX - size; index++;
		vertices[index] = randY - size; index++;
		vertices[index] = randZ; index++;
		vertices[index] = randVelX; index++;
		vertices[index] = randVelY; index++;
		vertices[index] = randVelZ; index++;
		vertices[index] = startTime; index++;
		vertices[index] = lifeTime; index++;

		vertices[index] = randX + size; index++;
		vertices[index] = randY - size; index++;
		vertices[index] = randZ; index++;
		vertices[index] = randVelX; index++;
		vertices[index] = randVelY; index++;
		vertices[index] = randVelZ; index++;
		vertices[index] = startTime; index++;
		vertices[index] = lifeTime; index++;

		vertices[index] = randX + size; index++;
		vertices[index] = randY + size; index++;
		vertices[index] = randZ; index++;
		vertices[index] = randVelX; index++;
		vertices[index] = randVelY; index++;
		vertices[index] = randVelZ; index++;
		vertices[index] = startTime; index++;
		vertices[index] = lifeTime; index++;
	}

	glGenBuffers(1, &mVBOQuads);
	glBindBuffer(GL_ARRAY_BUFFER, mVBOQuads);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (arraySize), vertices, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, _msize(vertices) / sizeof(*vertices), vertices, GL_STATIC_DRAW);
	mVBOQuadsVertexCount = countQuad * verticesPerQuad;
	delete[] vertices;
	vertices = NULL;
}

void Renderer::GenQuadsVBO(int num, bool bRandPos, GLuint* id, GLuint* vCount)
{
	float size = 0.03f;
	int countQuad = num;
	int verticesPerQuad = 6;
	int floatsPerVertex = 3 + 3 + 2 + 2 + 1 + 4; //x,y,z, vx,vy,vz, s,l,r,a, randValue, r,gb,a
	int arraySize = countQuad * verticesPerQuad * floatsPerVertex;
	float* vertices = new float[arraySize];

	for (int i = 0; i < countQuad; i++)
	{
		float randX, randY, randZ;
		float randVelX, randVelY, randVelZ;
		float startTime, lifeTime;
		float startTimeMax = 6.f, lifeTimeMax = 3.f;
		float ratio, amp;
		float ratioMin = 2.f, ratioMax = 4.f;
		float ampMin = -0.1f, ampMax = 0.2f;
		float randValue = 0.f, randThres = 1.f;
		float colourR, colourG, colourB, colourA;

		int index = i * verticesPerQuad * floatsPerVertex;
		if (bRandPos == true)
		{
			randX = 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
			randY = 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
			randZ = 0.f;
		}

		else
		{
			randX = 0.f;
			randY = 0.f;
			randZ = 0.f;
		}

		randVelX = 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
		randVelY = 2.f * (((float)rand() / (float)RAND_MAX) - 0.5f);
		randVelZ = 0.f;

		startTime = ((float)rand() / (float)RAND_MAX) * startTimeMax;
		lifeTime = ((float)rand() / (float)RAND_MAX) * lifeTimeMax;

		ratio = ratioMin + ((float)rand() / (float)RAND_MAX) * ratioMax;
		amp = ampMin + ((float)rand() / (float)RAND_MAX) * ampMax;

		randValue = randValue + ((float)rand() / (float)RAND_MAX) * randThres;

		colourR = ((float)rand() / (float)RAND_MAX);
		colourG = ((float)rand() / (float)RAND_MAX);
		colourB = ((float)rand() / (float)RAND_MAX);
		colourA = 1.f;

		vertices[index] = randX - size; index++;
		vertices[index] = randY - size; index++;
		vertices[index] = randZ; index++;
		vertices[index] = randVelX; index++;
		vertices[index] = randVelY; index++;
		vertices[index] = randVelZ; index++;
		vertices[index] = startTime; index++;
		vertices[index] = lifeTime; index++;
		vertices[index] = ratio; index++;
		vertices[index] = amp; index++;
		vertices[index] = randValue; index++;
		vertices[index] = colourR; index++;
		vertices[index] = colourG; index++;
		vertices[index] = colourB; index++;
		vertices[index] = colourA; index++;

		vertices[index] = randX - size; index++;
		vertices[index] = randY + size; index++;
		vertices[index] = randZ; index++;
		vertices[index] = randVelX; index++;
		vertices[index] = randVelY; index++;
		vertices[index] = randVelZ; index++;
		vertices[index] = startTime; index++;
		vertices[index] = lifeTime; index++;
		vertices[index] = ratio; index++;
		vertices[index] = amp; index++;
		vertices[index] = randValue; index++;
		vertices[index] = colourR; index++;
		vertices[index] = colourG; index++;
		vertices[index] = colourB; index++;
		vertices[index] = colourA; index++;

		vertices[index] = randX + size; index++;
		vertices[index] = randY + size; index++;
		vertices[index] = randZ; index++;
		vertices[index] = randVelX; index++;
		vertices[index] = randVelY; index++;
		vertices[index] = randVelZ; index++;
		vertices[index] = startTime; index++;
		vertices[index] = lifeTime; index++;
		vertices[index] = ratio; index++;
		vertices[index] = amp; index++;
		vertices[index] = randValue; index++;
		vertices[index] = colourR; index++;
		vertices[index] = colourG; index++;
		vertices[index] = colourB; index++;
		vertices[index] = colourA; index++;
		//-------------------------------------------//
		vertices[index] = randX - size; index++;
		vertices[index] = randY - size; index++;
		vertices[index] = randZ; index++;
		vertices[index] = randVelX; index++;
		vertices[index] = randVelY; index++;
		vertices[index] = randVelZ; index++;
		vertices[index] = startTime; index++;
		vertices[index] = lifeTime; index++;
		vertices[index] = ratio; index++;
		vertices[index] = amp; index++;
		vertices[index] = randValue; index++;
		vertices[index] = colourR; index++;
		vertices[index] = colourG; index++;
		vertices[index] = colourB; index++;
		vertices[index] = colourA; index++;

		vertices[index] = randX + size; index++;
		vertices[index] = randY - size; index++;
		vertices[index] = randZ; index++;
		vertices[index] = randVelX; index++;
		vertices[index] = randVelY; index++;
		vertices[index] = randVelZ; index++;
		vertices[index] = startTime; index++;
		vertices[index] = lifeTime; index++;
		vertices[index] = ratio; index++;
		vertices[index] = amp; index++;
		vertices[index] = randValue; index++;
		vertices[index] = colourR; index++;
		vertices[index] = colourG; index++;
		vertices[index] = colourB; index++;
		vertices[index] = colourA; index++;

		vertices[index] = randX + size; index++;
		vertices[index] = randY + size; index++;
		vertices[index] = randZ; index++;
		vertices[index] = randVelX; index++;
		vertices[index] = randVelY; index++;
		vertices[index] = randVelZ; index++;
		vertices[index] = startTime; index++;
		vertices[index] = lifeTime; index++;
		vertices[index] = ratio; index++;
		vertices[index] = amp; index++;
		vertices[index] = randValue; index++;
		vertices[index] = colourR; index++;
		vertices[index] = colourG; index++;
		vertices[index] = colourB; index++;
		vertices[index] = colourA; index++;
	}
	GLuint vboID = 0;

	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (arraySize), vertices, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, _msize(vertices) / sizeof(*vertices), vertices, GL_STATIC_DRAW);
	//mVBOQuadsVertexCount = countQuad * verticesPerQuad;
	*vCount = countQuad * verticesPerQuad;
	*id = vboID;
	delete[] vertices;
	vertices = NULL;
}

void Renderer::CreateGridMesh()
{
	float basePosX = -0.5f;
	float basePosY = -0.5f;
	float targetPosX = 0.5f;
	float targetPosY = 0.5f;

	int pointCountX = 32;
	int pointCountY = 32;

	float width = targetPosX - basePosX;
	float height = targetPosY - basePosY;

	float* point = new float[pointCountX * pointCountY * 2];
	float* vertices = new float[(pointCountX - 1) * (pointCountY - 1) * 2 * 3 * 3];
	mVBOGridMeshCount = (pointCountX - 1) * (pointCountY - 1) * 2 * 3;

	//Prepare points
	for (int x = 0; x < pointCountX; x++)
	{
		for (int y = 0; y < pointCountY; y++)
		{
			point[(y * pointCountX + x) * 2 + 0] = basePosX + width * (x / (float)(pointCountX - 1));
			point[(y * pointCountX + x) * 2 + 1] = basePosY + height * (y / (float)(pointCountY - 1));
		}
	}

	//Make triangles
	int vertIndex = 0;
	for (int x = 0; x < pointCountX - 1; x++)
	{
		for (int y = 0; y < pointCountY - 1; y++)
		{
			//Triangle part 1
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;

			//Triangle part 2
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + x) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[(y * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 0];
			vertIndex++;
			vertices[vertIndex] = point[((y + 1) * pointCountX + (x + 1)) * 2 + 1];
			vertIndex++;
			vertices[vertIndex] = 0.f;
			vertIndex++;
		}
	}

	glGenBuffers(1, &mVBOGridMesh);
	glBindBuffer(GL_ARRAY_BUFFER, mVBOGridMesh);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (pointCountX - 1) * (pointCountY - 1) * 2 * 3 * 3, vertices, GL_STATIC_DRAW);
}

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

unsigned char* Renderer::loadBMPRaw(const char* imagepath, unsigned int& outWidth, unsigned int& outHeight)
{
	std::cout << "Loading bmp file " << imagepath << " ... " << std::endl;
	outWidth = -1;
	outHeight = -1;
	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	// Actual RGB data
	unsigned char* data;

	// Open the file
	FILE* file = NULL;
	fopen_s(&file, imagepath, "rb");
	if (!file)
	{
		std::cout << "Image could not be opened, " << imagepath << " is missing. " << std::endl;
		return NULL;
	}

	if (fread(header, 1, 54, file) != 54)
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	if (header[0] != 'B' || header[1] != 'M')
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	if (*(int*)&(header[0x1E]) != 0)
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	if (*(int*)&(header[0x1C]) != 24)
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	outWidth = *(int*)&(header[0x12]);
	outHeight = *(int*)&(header[0x16]);

	if (imageSize == 0)
		imageSize = outWidth * outHeight * 3;

	if (dataPos == 0)
		dataPos = 54;

	data = new unsigned char[imageSize];

	fread(data, 1, imageSize, file);

	fclose(file);

	std::cout << imagepath << " is succesfully loaded. " << std::endl;

	return data;
}

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

GLuint Renderer::CreateBmpTexture(char* filePath)
{
	//Load Bmp: Load file and decode image.
	unsigned int width, height;
	unsigned char* bmp
		= loadBMPRaw(filePath, width, height);

	if (bmp == NULL)
	{
		std::cout << "Error while loading bmp file : " << filePath << std::endl;
		assert(bmp != NULL);
		return -1;
	}

	GLuint temp;
	glGenTextures(1, &temp);

	glBindTexture(GL_TEXTURE_2D, temp);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp);

	return temp;
}

float g_Time = 0;

void Renderer::Test()
{
	glUseProgram(m_SolidRectShader);

	GLuint uTime = glGetUniformLocation(m_SolidRectShader, "u_Time");
	glUniform1f(uTime, g_Time);
	g_Time += 0.01f;
	if (g_Time > 1.0f)
		g_Time = 0.f;

	GLuint aPos = glGetAttribLocation(m_SolidRectShader, "a_Position");
	GLuint aCol = glGetAttribLocation(m_SolidRectShader, "a_Colour");

	glEnableVertexAttribArray(aPos);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(aPos, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);

	glEnableVertexAttribArray(aCol);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORectColour);
	glVertexAttribPointer(aCol, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Renderer::Lecture02()
{
	glUseProgram(m_SolidRectShader);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mVBOQuads);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, mVBOQuadsVertexCount);

	glDisableVertexAttribArray(0);
}

void Renderer::Lecture03()
{
	glUseProgram(m_SolidRectShader);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mVBOGridMesh);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_LINE_STRIP, 0, mVBOGridMeshCount);

	glDisableVertexAttribArray(0);
}

void Renderer::Lecture04()
{
	glUseProgram(m_SimpleVelShader);

	GLuint uTime = glGetUniformLocation(m_SimpleVelShader, "u_Time");
	glUniform1f(uTime, g_Time);
	g_Time += 0.01f;

	GLuint aPos = glGetAttribLocation(m_SimpleVelShader, "a_Position");
	GLuint aVel = glGetAttribLocation(m_SimpleVelShader, "a_Vel");

	glEnableVertexAttribArray(aPos);
	glEnableVertexAttribArray(aVel);
	glBindBuffer(GL_ARRAY_BUFFER, mVBOQuads);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(aVel, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 3));

	glDrawArrays(GL_TRIANGLES, 0, mVBOQuadsVertexCount);
	//glDrawArrays(GL_LINE_STRIP, 0, mVBOQuadsVertextCount);	//디버그용

	glDisableVertexAttribArray(aPos);
	glDisableVertexAttribArray(aVel);
}

void Renderer::SimpleVel()
{
	glUseProgram(m_SimpleVelShader);

	GLuint uTime = glGetUniformLocation(m_SimpleVelShader, "u_Time");
	GLuint uRepeat = glGetUniformLocation(m_SimpleVelShader, "u_Repeat");
	glUniform1f(uTime, g_Time);
	g_Time += 0.01f;

	GLuint aPos = glGetAttribLocation(m_SimpleVelShader, "a_Position");
	GLuint aVel = glGetAttribLocation(m_SimpleVelShader, "a_Vel");
	GLuint aStartLife = glGetAttribLocation(m_SimpleVelShader, "a_StartLife");

	glEnableVertexAttribArray(aPos);
	glEnableVertexAttribArray(aVel);
	glEnableVertexAttribArray(aStartLife);

	glBindBuffer(GL_ARRAY_BUFFER, mVBOQuads);

	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, 0);
	glVertexAttribPointer(aVel, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(aStartLife, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(sizeof(float) * 6));

	glDrawArrays(GL_TRIANGLES, 0, mVBOQuadsVertexCount);
	//glDrawArrays(GL_LINE_STRIP, 0, mVBOQuadsVertextCount);	//디버그용

	glDisableVertexAttribArray(aPos);
	glDisableVertexAttribArray(aVel);
	glDisableVertexAttribArray(aStartLife);
}

void Renderer::SimpleParticle()
{
	GLuint shader = m_SimpleParticleShader;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(shader);

	GLuint uTime = glGetUniformLocation(shader, "u_Time");
	glUniform1f(uTime, g_Time);
	g_Time += 0.01f;

	GLuint uniformTex = glGetUniformLocation(shader, "u_Texture");
	glUniform1i(uniformTex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ParticleTexture);

	GLuint aPos = glGetAttribLocation(shader, "a_Position");
	GLuint aVel = glGetAttribLocation(shader, "a_Velocity");
	GLuint aSLRA = glGetAttribLocation(shader, "a_StartLifeRatioAmp");
	GLuint aRV = glGetAttribLocation(shader, "a_RandValue");
	GLuint aC = glGetAttribLocation(shader, "a_Colour");

	glEnableVertexAttribArray(aPos);
	glEnableVertexAttribArray(aVel);
	glEnableVertexAttribArray(aSLRA);
	glEnableVertexAttribArray(aRV);
	glEnableVertexAttribArray(aC);

	glBindBuffer(GL_ARRAY_BUFFER, mVBOQuads);

	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, 0);
	glVertexAttribPointer(aVel, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(aSLRA, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(aRV, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (GLvoid*)(sizeof(float) * 10));
	glVertexAttribPointer(aC, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 15, (GLvoid*)(sizeof(float) * 11));

	glDrawArrays(GL_TRIANGLES, 0, mVBOQuadsVertexCount);

	glDisableVertexAttribArray(aPos);
	glDisableVertexAttribArray(aVel);
	glDisableVertexAttribArray(aSLRA);
	glDisableVertexAttribArray(aRV);
	glDisableVertexAttribArray(aC);
}

void Renderer::Lecture07()
{
	GLfloat points[] = { 0,0, 0.5,0.5, 0.3,0.3, -0.2,-0.2, -0.3,-0.3 };

	GLuint shader = m_FragmentTestShader;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(shader);

	GLuint uPoints = glGetUniformLocation(shader, "u_Points");
	glUniform2fv(uPoints, 5, points);

	GLuint uTime = glGetUniformLocation(shader, "u_Time");
	glUniform1f(uTime, g_Time);
	g_Time += 0.005f;

	GLuint aPos = glGetAttribLocation(shader, "a_Position");
	GLuint aUV = glGetAttribLocation(shader, "a_UV");

	glEnableVertexAttribArray(aPos);
	glEnableVertexAttribArray(aUV);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);

	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(aUV, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 4));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(aPos);
}

void Renderer::FillAll(float alpha)
{
	GLuint shader = m_FillAllShader;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(shader);

	GLuint aPos = glGetAttribLocation(shader, "a_Position");

	glEnableVertexAttribArray(aPos);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);

	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(aPos);
}

void Renderer::DrawRectTex(GLuint Tex)
{
	//GLuint shader = m_RectTexShader;
	GLuint shader = m_TexMappingShader;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(shader);

	GLuint uTime = glGetUniformLocation(shader, "u_Time");
	glUniform1f(uTime, g_Time);
	g_Time += 0.01f;

	GLuint uTex = glGetUniformLocation(shader, "u_Texture");
	glUniform1i(uTex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureParameter2D);

	GLuint aPos = glGetAttribLocation(shader, "a_Position");
	GLuint aTex = glGetAttribLocation(shader, "a_Tex");

	glEnableVertexAttribArray(aPos);
	glEnableVertexAttribArray(aTex);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBORectTex);

	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(aTex, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(aPos);
	glDisableVertexAttribArray(aTex);
}

// 합성곱 필터 적용 함수
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

void Renderer::ApplyFilterAndUpdateTexture()
{
	Image img;
	glBindTexture(GL_TEXTURE_2D, m_TextureParameter2D);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&img.width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&img.height);
	img.data.resize(img.width * img.height * 4);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data.data());

	Image filteredImg = EdgeDetection(img);

	GLuint newTexture;
	glGenTextures(1, &newTexture);
	glBindTexture(GL_TEXTURE_2D, newTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, filteredImg.width, filteredImg.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, filteredImg.data.data());

	glDeleteTextures(1, &m_TextureParameter2D);
	m_TextureParameter2D = newTexture;
}

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
	switch (0)
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