#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "glm\glm.hpp"

#include "UIManager.h"
#include "SceneGraph.h"

class GameManager;

class MapEditor
{
private:
	GLFWwindow* _window;

	Camera* _camera;
	SceneGraph* _sceneGraph;
	UIManager* _uiManager;
	GameManager* _gameManager;
	Object* _selectedObject;

	int _screenWidth, _screenHeight;
	double _mouseX, _mouseY;
	const float _cMouseSpeed;

	bool _parentingObjects = false;
public:
	MapEditor(GameManager* gameManager, GLFWwindow* window, UIManager* uiManager, SceneGraph* sceneGraph, Camera* camera);
	~MapEditor();

	void LoadNewMap();

	void StartMapEditor();

	void Update();

	void Draw();

	void CalculateCameraAngles();

	void HandleInputs();
};

