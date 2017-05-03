#pragma once
#include "Billboard.h"
class PointLight : public Billboard
{
private:
	PntLight* _light;
public:
	PointLight(Texture2D* texture, PntLight* light, glm::vec3 position);
	~PointLight();

	PntLight* GetLightParameters() { return _light; };
};

