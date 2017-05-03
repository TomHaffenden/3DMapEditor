#pragma once
#include "SceneObject.h"

class DynamicObject : public SceneObject
{
protected:
	glm::vec3 _velocity;
	glm::vec3 _rotationVelocity;
public:
	DynamicObject(VBO* vbo, Material* material, glm::vec3 position);
	DynamicObject(const DynamicObject &object);
	~DynamicObject();

	void Update(glm::mat4& parentMatrix);
	void Draw();

	glm::vec3 GetVelocity() { return _velocity; };
	glm::vec3 GetRotationVelocity() { return glm::vec3(_rotationVelocity.x * (180 / PI), _rotationVelocity.y * (180 / PI), _rotationVelocity.z * (180 / PI)); };
	glm::vec3 GetRawRotationVelocity() { return _rotationVelocity; };

	void SetVelocity(GLfloat x, GLfloat y, GLfloat z);
	void SetVelocity(glm::vec3 velocity) { _velocity = velocity; };
	void SetRotationVelocity(GLfloat x, GLfloat y, GLfloat z);
	void SetRotationVelocity(glm::vec3 rotationVelocity) { _rotationVelocity = rotationVelocity; };
};

