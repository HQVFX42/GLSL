#pragma once

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "Dependencies\glew.h"
#include "Dependencies\wglew.h"
#include "Dependencies\glm/glm.hpp"
#include "Dependencies\glm/gtc/matrix_transform.hpp"
#include "Dependencies\glm/gtx/euler_angles.hpp"

struct Image
{
	std::vector<unsigned char> data;
	unsigned int width, height;
};

class Renderer
{
public:
	Renderer(int windowSizeX, int windowSizeY);
	~Renderer();

	GLuint CreatePngTexture(char * filePath);
	GLuint CreateBmpTexture(char * filePath);
	   
	void Test();
	void Lecture02();
	void Lecture03();
	void Lecture04();
	void SimpleVel();
	void SimpleParticle();
	void Lecture07();
	void FillAll(float alpha);
	void DrawRectTex(GLuint Tex);

	/**
	 * Image Filtering.
	 */

	Image ApplyConvolution(const Image& src, const std::vector<float>& kernel, int kernelSize);
	Image Sharpen(const Image& src);
	Image EdgeDetection(const Image& src);
	Image Blur(const Image& src);
	void ApplyFilterAndUpdateTexture();
	void GPUConvolutionFilter();

private:
	void Initialize(int windowSizeX, int windowSizeY);
	bool ReadFile(char* filename, std::string *target);
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	GLuint CompileShaders(char* filenameVS, char* filenameFS);
	void CreateVertexBufferObjects();
	void GenQuads(int num);
	void GenQuadsVBO(int num, bool bRandPos, GLuint * id, GLuint * vCount);
	void CreateGridMesh();
	void CreateTextures();
	unsigned char * Renderer::loadBMPRaw(const char * imagepath, unsigned int& outWidth, unsigned int& outHeight);

	bool m_Initialized = false;
	
	unsigned int m_WindowSizeX = 0;
	unsigned int m_WindowSizeY = 0;

	GLuint m_VBORect = 0;
	GLuint m_VBORectColour = 0;
	GLuint mVBOTriangles = 0;
	GLuint mVBOQuads = 0;
	GLuint mVBOQuadsVertexCount = 0;
	GLuint mVBOGridMesh = 0;
	int mVBOGridMeshCount = 0;
	GLuint m_VBORectTex = 0;
	GLuint m_checkerboardTexture = 0;

	//Shaders
	GLuint m_SolidRectShader = 0;
	GLuint m_SimpleVelShader = 0;
	GLuint m_SimpleParticleShader = 0;
	GLuint m_FragmentTestShader = 0;
	GLuint m_FillAllShader = 0;
	GLuint m_RectTexShader = 0;
	GLuint m_TexMappingShader = 0;

	//Textures
	GLuint m_ParticleTexture = 0;
	GLuint m_Particle1Texture = 0;
	GLuint m_Particle2Texture = 0;
	GLuint m_TextureParameter2D = 0;
	GLuint m_BackGroundTexture = 0;

	//Test GPU computing
	GLuint m_ComputeShader = 0;
	GLuint m_ComputeProgram = 0;
	GLuint m_OutputTexture = 0;
};

