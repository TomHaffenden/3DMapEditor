#include "StaticTextured.h"



StaticTextured::StaticTextured(StaticObject* object, Texture2D* texture, Texture2D* specular) : StaticObject(*object)
{
	_shader = &ShaderManager::textureShader;
	_texture = texture;
	_textureSpecular = specular;

	_type = ObjectType::STATIC_TEXTURED_OBJECT;
	_name = "Static Textured Object";
}


StaticTextured::~StaticTextured()
{
}

void StaticTextured::Update(glm::mat4& parentMatrix)
{
	StaticObject::Update(parentMatrix);
}

void StaticTextured::Draw()
{
	glUseProgram(_shader->shaderID);

	glUniform1f(glGetUniformLocation(_shader->shaderID, "material.shininess"), _material->shininess);

	glUniformMatrix4fv(_modelMatrixID, 1, GL_FALSE, &_modelMatrix[0][0]);

	glm::mat4 MVP = MatrixHolder::projectionMatrix * MatrixHolder::viewMatrix * (_modelMatrix * _scaleMatrix);
	glUniformMatrix4fv(_shader->matrixID, 1, GL_FALSE, &MVP[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture->GetTextureID());
	glUniform1i(_texture->GetTextureSampler(), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _textureSpecular->GetTextureID());
	glUniform1i(_textureSpecular->GetTextureSampler(), 1);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo->vertexArrayID);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo->uvArrayID);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo->normalArrayID);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo->elementArrayID);

	glDrawElements(
		GL_TRIANGLES,
		_vbo->indicesSize,
		GL_UNSIGNED_SHORT,
		(void*)0
	);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void StaticTextured::ChangeTexture(Texture2D* texture, Texture2D* specular)
{
	_texture = texture;
	_textureSpecular = specular;
}