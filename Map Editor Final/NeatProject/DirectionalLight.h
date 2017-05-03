#pragma once

#include "Billboard.h"

class DirectionalLight : public Billboard
{
private:
	DirLight* _light;
public:
	DirectionalLight(Texture2D* texure, DirLight* light, glm::vec3 position);
	~DirectionalLight();

	DirLight* GetLightParameters() { return _light; };
};

