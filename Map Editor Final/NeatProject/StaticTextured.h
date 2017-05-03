#pragma once
#include "StaticObject.h"
class StaticTextured :
	public StaticObject
{
private:
	Texture2D* _texture, *_textureSpecular;
public:
	StaticTextured(StaticObject* object, Texture2D* texture, Texture2D* specular);
	~StaticTextured();

	Texture2D* GetTexture() { return _texture; };

	void Update(glm::mat4& parentMatrix);
	void Draw();

	void ChangeTexture(Texture2D* texture, Texture2D* specular);
};

