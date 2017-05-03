#pragma once
#include "SceneObject.h"


class StaticObject : public SceneObject
{
public:
	StaticObject(VBO* mesh, Material* material, glm::vec3 position);
	~StaticObject();

	void Update(glm::mat4 &parentMatrix);
	void Draw();
};