#include "Texture2D.h"

#include <stdio.h>

Texture2D::Texture2D()
{
}


Texture2D::~Texture2D()
{

}

bool Texture2D::LoadBMP(const char* path)
{
	printf("Reading image %s\n", path);
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned char * data;

	FILE * file = fopen(path, "rb");
	if (!file) {
		printf("%s could not be opened!", path);
		return false;
	}


	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return false;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return false;
	}

	// Making sure image is 24bit
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file.");    return false; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file.");    return false; }

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	_width = *(int*)&(header[0x12]);
	_height = *(int*)&(header[0x16]);

	if (imageSize == 0)    imageSize = _width*_height * 3;
	if (dataPos == 0)      dataPos = 54;

	data = new unsigned char[imageSize];

	fread(data, 1, imageSize, file);

	fclose(file);

	glGenTextures(1, &_ID);

	glBindTexture(GL_TEXTURE_2D, _ID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	delete[] data;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

bool Texture2D::LoadTGA(const char* path)
{
	unsigned char header[18];
	unsigned int imageSize;
	unsigned char imageType, pixelDepth, mode;
	unsigned char* data;

	FILE* file = fopen(path, "rb");
	if (!file) {
		printf("Image could not be opened.");
		return false;
	}
	if (fread(header, 1, 18, file) != 18)
	{
		printf("Not correct TGA file\n");
		return false;
	}

	imageType = header[0x02];
	_width = ((int)(header[0x0D]) << 8U) + (int)(header[0x0C]);
	_height = ((int)(header[0x0F]) << 8U) + (int)(header[0x0E]);
	pixelDepth = header[0x10];

	mode = pixelDepth / 8;
	imageSize = _width * _height * mode;

	data = new unsigned char[imageSize];

	fread(data, 1, imageSize, file);

	fclose(file);

	// Checking if image is RBG
	if (imageType == 2)
	{
		glGenTextures(1, &_ID);
		glBindTexture(GL_TEXTURE_2D, _ID);

		if (mode == 4) 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	}

	delete[] data;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

void Texture2D::SetTextureSampler(const GLuint programID, const char* path)
{
	_textureSampler = glGetUniformLocation(programID, path);
}