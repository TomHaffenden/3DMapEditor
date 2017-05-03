#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "glm\glm.hpp"

#include "common\Structures.h"
#include "SceneGraph.h"
#include "IOFileManager.h"

class GameManager;

class PlayState
{
private:
	GLFWwindow* _window;

	Camera* _camera;
	SceneGraph* _sceneGraph;
	GameManager* _gameManager;
	JSONIOFileManager* _io;

	glm::vec3 _cameraAABBmax;
	glm::vec3 _cameraAABBmin;

	int _maxObjects;
	int _currentObjects;
	float _score;

	int _screenWidth, _screenHeight;
	double _mouseX, _mouseY;
	const float _cMouseSpeed;
public:
	PlayState(GameManager* gameManager, GLFWwindow* window, Camera* camera);
	~PlayState();

	void StartPlayState();

	void CollisionCheck(SceneObject* object);

	void Update();

	void Draw();

	void CalculateCameraAngles();
	void HandleInputs(float deltaTime);
};

