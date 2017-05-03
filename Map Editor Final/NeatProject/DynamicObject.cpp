#include "DynamicObject.h"

#include "glm\gtc\matrix_transform.hpp"

DynamicObject::DynamicObject(VBO* vbo, Material* material, glm::vec3 position) : SceneObject(vbo, material, position)
{
	_velocity = glm::vec3(0.0f);
	_rotationVelocity = glm::vec3(0.0f);
	_type = ObjectType::DYNAMIC_MATERIAL_OBJECT;
	_name = "Dynamic Object";
}

DynamicObject::DynamicObject(const DynamicObject &object) : SceneObject(object._vbo, object._vbo->material, object._modelPosition)
{
	_velocity = object._velocity;
	_rotationVelocity = object._rotationVelocity;
	_type = ObjectType::DYNAMIC_MATERIAL_OBJECT;
}

DynamicObject::~DynamicObject()
{
}

void DynamicObject::Update(glm::mat4& parentMatrix)
{
	if (_rotationVelocity != glm::vec3(0.0f))
	{
		_rotationAngle += _rotationVelocity * GameStateManager::deltaTime;
		if (_rotationAngle.x > PI) {
			_rotationAngle.x = -PI;
		}
		else if (_rotationAngle.x < -PI) {
			_rotationAngle.x = PI;
		}
		if (_rotationAngle.y > PI) {
			_rotationAngle.y = -PI;
		}
		else if (_rotationAngle.y < -PI) {
			_rotationAngle.y = PI;
		}
		if (_rotationAngle.z > PI) {
			_rotationAngle.z = -PI;
		}
		else if (_rotationAngle.z < -PI) {
			_rotationAngle.z = PI;
		}
		_rotationMatrix = glm::rotate(glm::mat4(), _rotationAngle.x, glm::vec3(1.0f, 0.0f, 0.0f));
		_rotationMatrix = glm::rotate(_rotationMatrix, _rotationAngle.y, glm::vec3(0.0f, 1.0f, 0.0f));
		_rotationMatrix = glm::rotate(_rotationMatrix, _rotationAngle.z, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (GameStateManager::gameState == GameState::GamePlayState) {
		if (_velocity != glm::vec3(0.0f))
		{
			_translationMatrix = glm::translate(_translationMatrix, _velocity * GameStateManager::deltaTime);
		}
	}
	UpdateModelMatrix();

	Object::Update(parentMatrix);
}

void DynamicObject::Draw()
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

void DynamicObject::SetVelocity(GLfloat x, GLfloat y, GLfloat z)
{
	_velocity = glm::vec3(x, y, z);
}

void DynamicObject::SetRotationVelocity(GLfloat x, GLfloat y, GLfloat z)
{
	_rotationVelocity = glm::vec3(x*(PI / 180), y*(PI / 180), z*(PI / 180));
}