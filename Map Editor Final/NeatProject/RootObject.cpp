#include "RootObject.h"



RootObject::RootObject() : Object(glm::vec3(0))
{
	_type = ObjectType::ROOT;
}


RootObject::~RootObject()
{
}

void RootObject::Update(glm::mat4& parentMatrix)
{

}

void RootObject::Draw()
{

}