#pragma once

#include "SceneObject.h"
#include "glm\gtc\matrix_transform.hpp"

SceneObject::SceneObject(VBO* vbo, Material* material, glm::vec3 position) : Object(position)
{
	_shader = &ShaderManager::materialShader;;

	_vbo = vbo;
	_material = new Material(*material);

	_scaleFactor = glm::vec3(1.0f);

	_modelMatrix = glm::translate(glm::mat4(), _modelPosition);
	if(_vbo != nullptr)	_modelMatrixID = glGetUniformLocation(_shader->shaderID, "M");
}


SceneObject::~SceneObject()
{
	_vbo = nullptr;

	delete _material;
	_material = nullptr;
}

void SceneObject::UpdateModelMatrix()
{
	_modelMatrix = _translationMatrix * _rotationMatrix;
	return;
}

void SceneObject::GetAABB(glm::vec3 &max, glm::vec3 &min)
{
	max = glm::vec3(_vbo->maxX, _vbo->maxY, _vbo->maxZ);
	min = glm::vec3(_vbo->minX, _vbo->minY, _vbo->minZ);
	return;
}

void SceneObject::SetModelPosition(glm::vec3 position)
{
	_modelPosition = position;
	_translationMatrix = glm::translate(glm::mat4(), _modelPosition);
	UpdateModelMatrix();
	return;
}

void SceneObject::SetRotationAngle(GLfloat x, GLfloat y, GLfloat z)
{
	SetRotationAngle(glm::vec3(x*(PI / 180), y*(PI / 180), z*(PI / 180)));
	return;
}

void SceneObject::SetRotationAngle(glm::vec3 rotationAngle)
{ 
	_rotationAngle = rotationAngle;
	_rotationMatrix = glm::rotate(glm::mat4(), _rotationAngle.x, glm::vec3(1.0f, 0.0f, 0.0f));
	_rotationMatrix = glm::rotate(_rotationMatrix, _rotationAngle.y, glm::vec3(0.0f, 1.0f, 0.0f));
	_rotationMatrix = glm::rotate(_rotationMatrix, _rotationAngle.z, glm::vec3(0.0f, 0.0f, 1.0f));
	UpdateModelMatrix();
	return;
};

void SceneObject::SetScaleFactor(glm::vec3 scaleFactor)
{
	_scaleFactor = scaleFactor;
	_scaleMatrix = glm::scale(glm::mat4(), _scaleFactor);
	UpdateModelMatrix();
}