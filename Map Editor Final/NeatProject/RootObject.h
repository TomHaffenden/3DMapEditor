#pragma once
#include "Object.h"
class RootObject :
	public Object
{
public:
	RootObject();
	~RootObject();

	void Update(glm::mat4& parentMatrix);
	void Draw();
};

