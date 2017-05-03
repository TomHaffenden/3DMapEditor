#pragma once
#include "DynamicObject.h"
class DynamicTextured : public DynamicObject
{
private:
	Texture2D* _texture, *_textureSpecular;
public:
	DynamicTextured(DynamicObject* object, Texture2D* texture, Texture2D* textureSpecular);

	~DynamicTextured();

	Texture2D* GetTexture() { return _texture; };

	void Update(glm::mat4& parentMatrix);
	void Draw();

	void ChangeTexture(Texture2D* texture, Texture2D* specular);
};

