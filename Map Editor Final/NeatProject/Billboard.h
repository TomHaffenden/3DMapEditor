#pragma once

#include "Object.h"

class Billboard : public Object
{
private:
	Texture2D* _texture;
	GLuint _vertexBufferID;
	GLuint _cameraRightWorldSpaceID, _cameraUpWorldSpaceID, _matrixID, _billboardPosID, _billboardSizeID;

	glm::mat4 _parentMatrix;
public:
	Billboard(Texture2D* texure, glm::vec3 position);
	~Billboard();

	void Update(glm::mat4& parentMatrix);
	void Draw();

	Texture2D* GetTexture()
	{
		return _texture;
	}

	glm::mat4 GetParentMatrix() { return _parentMatrix; };
};

