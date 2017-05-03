#include "PointLight.h"



PointLight::PointLight(Texture2D* texture, PntLight* light, glm::vec3 position) : Billboard(texture, position)
{
	_light = light;
	_type = ObjectType::POINT_LIGHT;
	_name = "Point Light";
}


PointLight::~PointLight()
{
}