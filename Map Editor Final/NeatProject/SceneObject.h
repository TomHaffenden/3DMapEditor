#pragma once

#include "Object.h"

class SceneObject : public Object
{
protected:
	LightingShader* _shader;

	VBO* _vbo;
	Material* _material;
	glm::vec3 _rotationAngle, _scaleFactor;

	GLuint _modelMatrixID;
	glm::mat4 _rotationMatrix, _scaleMatrix;
public:
	SceneObject(VBO* vbo, Material* material, glm::vec3 position);
	virtual ~SceneObject();

	virtual void Update(glm::mat4& parentMatrix) = 0;
	virtual void Draw() = 0;

	VBO* GetVertexBufferObject() { return _vbo; };
	GLuint GetModelMatrixID() { return _modelMatrixID; };
	glm::vec3 GetRotationAngle() { return glm::vec3(_rotationAngle.x * (180/PI), _rotationAngle.y * (180/PI), _rotationAngle.z * (180/PI)); };
	glm::vec3 GetRawRotationAngle() { return _rotationAngle; };
	glm::vec3 GetScaleFactor() { return _scaleFactor; };
	glm::mat4 GetScaleMatrix() { return _scaleMatrix; };
	virtual void GetAABB(glm::vec3 &max, glm::vec3 &min);
	Material* GetMaterial() { return _material; };
	LightingShader* GetShader() { return _shader; };

	void SetModelPosition(glm::vec3 modelPosition);

	void UpdateModelMatrix();

	void SetRotationAngle(GLfloat x, GLfloat y, GLfloat z);
	void SetRotationAngle(glm::vec3 rotationAngle);

	void SetScaleFactor(glm::vec3 scaleFactor);
};