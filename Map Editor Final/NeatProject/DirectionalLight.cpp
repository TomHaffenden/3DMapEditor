#include "DirectionalLight.h"



DirectionalLight::DirectionalLight(Texture2D* texure, DirLight* light, glm::vec3 position) : Billboard(texure, position)
{
	_light = light;
	_type = ObjectType::DIRECTIONAL_LIGHT;
	_name = "Directional Light";
}


DirectionalLight::~DirectionalLight()
{
	delete _light;
	_light = nullptr;
}