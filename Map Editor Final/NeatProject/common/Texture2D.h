#pragma once

#include <GL\glew.h>
#include <string>

using namespace std;

class Texture2D
{
private:
	GLuint _ID, _textureSampler;
	int _width, _height;
	string _texturePath;
public:
	Texture2D();
	~Texture2D();

	bool LoadBMP(const char* path);
	bool LoadTGA(const char* path);

	void SetTextureSampler(const GLuint programID, const char* path);
	
	GLuint GetTextureID()
	{
		return _ID;
	}

	GLuint GetTextureSampler()
	{
		return _textureSampler;
	}

	int GetWidth()
	{
		return _width;
	}

	int GetHeight()
	{
		return _height;
	}

	string* GetTexturePath() { return &_texturePath; };
};

