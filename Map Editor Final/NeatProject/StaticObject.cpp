#pragma once

#include "StaticObject.h"
#include "glm\gtc\matrix_transform.hpp"

StaticObject::StaticObject(VBO* vbo, Material* material, glm::vec3 position) : SceneObject(vbo, material, position)
{
	_type = ObjectType::STATIC_MATERIAL_OBJECT;
	_name = "Static Object";
}

StaticObject::~StaticObject()
{

}

void StaticObject::Update(glm::mat4 &parentMatrix)
{
	UpdateModelMatrix();
	Object::Update(parentMatrix);
}

void StaticObject::Draw()
{
	glUseProgram(_shader->shaderID);

	glUniform3f(glGetUniformLocation(_shader->shaderID, "material.ambient"), _material->ambient.x, _material->ambient.y, _material->ambient.z);
	glUniform3f(glGetUniformLocation(_shader->shaderID, "material.diffuse"), _material->diffuse.x, _material->diffuse.y, _material->diffuse.z);
	glUniform3f(glGetUniformLocation(_shader->shaderID, "material.specular"), _material->specular.x, _material->specular.y, _material->specular.z);
	glUniform1f(glGetUniformLocation(_shader->shaderID, "material.shininess"), _material->shininess);

	glUniformMatrix4fv(_modelMatrixID, 1, GL_FALSE, &_modelMatrix[0][0]);

	glm::mat4 MVP = MatrixHolder::projectionMatrix * MatrixHolder::viewMatrix * (_modelMatrix * _scaleMatrix);
	glUniformMatrix4fv(_shader->matrixID, 1, GL_FALSE, &MVP[0][0]);

	if (_vbo->indexed) {
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo->vertexArrayID);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo->normalArrayID);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo->elementArrayID);

		glDrawElements(
			GL_TRIANGLES,
			_vbo->indicesSize,
			GL_UNSIGNED_SHORT,
			(void*)0
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


	}
	else {
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo->vertexArrayID);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo->normalArrayID);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glDrawArrays(GL_TRIANGLES, 0, _vbo->indicesSize);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
}