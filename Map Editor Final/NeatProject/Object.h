#pragma once

#include "common\Structures.h"
#include "common\Texture2D.h"

#include "glm\gtc\matrix_transform.hpp"

class Object
{
protected:
	BasicShader* _shader;
	ObjectType _type;
	string _name;

	glm::vec3 _modelPosition;

	glm::mat4 _modelMatrix, _translationMatrix;
public:
	Object(glm::vec3 position);
	~Object();

	virtual void Update(glm::mat4 &parentMatrix) = 0;
	virtual void Draw() = 0;

	glm::mat4 GetModelMatrix() { return _modelMatrix; };
	glm::vec3 GetModelPosition() { return _modelPosition; };
	string* GetName() { return &_name; };
	ObjectType GetType() { return _type; };
	BasicShader* GetShader() { return _shader; };
	GLuint GetShaderID() { return _shader->shaderID; };

	void SetName(string name) { _name = name; };

	void SetModelPosition(glm::vec3 modelPosition);

	void UpdateModelMatrix();
};

