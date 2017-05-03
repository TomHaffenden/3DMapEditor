#include "Billboard.h"

Billboard::Billboard(Texture2D* texture, glm::vec3 position) : Object(position)
{
	_texture = texture;

	_shader = &ShaderManager::billboardShader;

	_cameraRightWorldSpaceID = glGetUniformLocation(_shader->shaderID, "cameraRight_worldSpace");
	_cameraUpWorldSpaceID = glGetUniformLocation(_shader->shaderID, "cameraUp_worldSpace");
	_matrixID = glGetUniformLocation(_shader->shaderID, "VP");
	_billboardPosID = glGetUniformLocation(_shader->shaderID, "billboardPos");
	_billboardSizeID = glGetUniformLocation(_shader->shaderID, "billboardSize");

	static const GLfloat g_vertex_buffer_data[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	};

	glGenBuffers(1, &_vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);
}


Billboard::~Billboard()
{
}

void Billboard::Update(glm::mat4& parentMatrix)
{
	_parentMatrix = parentMatrix;
	Object::Update(parentMatrix);
}

void Billboard::Draw()
{
	if (GameStateManager::gameState == GameState::MapEditorState) {
		glm::mat4 VP = MatrixHolder::projectionMatrix * MatrixHolder::viewMatrix;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(_shader->shaderID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture->GetTextureID());
		glUniform1i(_texture->GetTextureSampler(), 0);

		glUniform3f(_cameraRightWorldSpaceID, MatrixHolder::viewMatrix[0][0], MatrixHolder::viewMatrix[1][0], MatrixHolder::viewMatrix[2][0]);
		glUniform3f(_cameraUpWorldSpaceID, MatrixHolder::viewMatrix[0][1], MatrixHolder::viewMatrix[1][1], MatrixHolder::viewMatrix[2][1]);

		glm::vec4 worldPosition = _parentMatrix * glm::vec4(_modelPosition, 1.0f);

		glUniform3f(_billboardPosID, worldPosition.x, worldPosition.y, worldPosition.z);
		glUniform2f(_billboardSizeID, 0.5f, 0.5f);

		glUniformMatrix4fv(_matrixID, 1, GL_FALSE, &VP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(0);

		glDisable(GL_BLEND);
	}
}