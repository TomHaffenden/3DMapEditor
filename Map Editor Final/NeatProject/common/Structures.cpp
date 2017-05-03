#include "Structures.h"

#include "../DirectionalLight.h"
#include "../PointLight.h"

namespace MatrixHolder
{
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
}

namespace ShaderManager
{
	LightingShader materialShader;
	LightingShader textureShader;
	BasicShader billboardShader;
}

namespace LightManager
{
	DirectionalLight* directionalLight;
	PointLight* pointLights[4];
}

namespace GameStateManager
{
	GameState gameState;
	float deltaTime;
}

namespace FileManager
{
	std::string currentFilePath;
}