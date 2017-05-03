#include "Object.h"

Object::Object(glm::vec3 position)
{
	_shader = NULL;
	_modelPosition = position;
	_translationMatrix = glm::translate(glm::mat4(), _modelPosition);
	_modelMatrix = _translationMatrix;
	_name = "Root";
}


Object::~Object()
{
}


void Object::Update(glm::mat4& parentMatrix)
{
	_modelMatrix = parentMatrix * _modelMatrix;
}

void Object::SetModelPosition(glm::vec3 position)
{
	_modelPosition = position;
	_translationMatrix = glm::translate(glm::mat4(), _modelPosition);
	UpdateModelMatrix();
	return;
}

void Object::UpdateModelMatrix()
{
	_modelMatrix = _translationMatrix;
}