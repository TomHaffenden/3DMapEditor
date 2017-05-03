#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#include "Object.h"

#include "Billboard.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#include "SceneObject.h"
#include "DynamicObject.h"
#include "DynamicTextured.h"
#include "StaticObject.h"

#include "IOFileManager.h"
#include "UIManager.h"
#include "SceneGraph.h"

#include "MapEditor.h"
#include "PlayState.h"

class GameManager
{
private:
	int _screenWidth, _screenHeight;
	GLFWwindow* _window;
	SceneGraph* _sceneGraph;
	UIManager* _uiManager;

	MapEditor* _mapEditor;
	PlayState* _playState;

	GLuint _vertexArrayID;

	map<string, Texture2D*> _textures;
	map<string, Texture2D*> _speculars;
	map<string, VBO*> _meshes;

	Camera* _camera;
public:
	GameManager();
	~GameManager();

	int InitOpenGL();
	void LoadResources();

	void LoadNewMap();
	void SwitchGameState(GameState gameState);

	void LoadShader(LightingShader& shader);
	Texture2D* LoadTexture(string path);
	Texture2D* LoadSpecular(string path);
	VBO* LoadMesh(string path);

	void Update();

	void Draw();

	void FillShader(LightingShader& shader);

	void CloseGame() { glfwSetWindowShouldClose(_window, true); };
};

