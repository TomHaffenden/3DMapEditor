#include "MapEditor.h"

#include "glm\gtc\matrix_transform.hpp"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_glfw_gl3.h"

#include "GameManager.h"

MapEditor::MapEditor(GameManager* gameManager, GLFWwindow* window, UIManager* uiManager, SceneGraph* sceneGraph, Camera* camera) : _cMouseSpeed(0.005f)
{
	_gameManager = gameManager;
	_window = window;
	_uiManager = uiManager;
	_sceneGraph = sceneGraph;
	_camera = camera;
	glfwGetWindowSize(_window, &_screenWidth, &_screenHeight);
	glfwGetCursorPos(_window, &_mouseX, &_mouseY);

	CalculateCameraAngles();

	LoadNewMap();
}

MapEditor::~MapEditor()
{
}

void MapEditor::LoadNewMap()
{
	DirLight* dirLight = new DirLight();
	dirLight->ambient = glm::vec3(0.1f);
	dirLight->diffuse = glm::vec3(0.3f);
	dirLight->specular = glm::vec3(0.5f, 0.5f, 0.5f);

	LightManager::directionalLight = new DirectionalLight(_gameManager->LoadTexture("2D/sunBillboard.tga"), dirLight, glm::vec3(0.0f, 4.0f, 0.0f));
	_sceneGraph->AddRootSibling(LightManager::directionalLight);

	for (int i = 0; i < 4; i++)
	{
		PntLight* pntLight = new PntLight();
		pntLight->ambient = glm::vec3(0);
		pntLight->diffuse = glm::vec3(0);
		pntLight->specular = glm::vec3(0);

		pntLight->constant = 1.0f;
		pntLight->linear = 0.045f;
		pntLight->quadratic = 0.0075f;

		LightManager::pointLights[i] = new PointLight(_gameManager->LoadTexture("2D/lightBillboard.tga"), pntLight, glm::vec3(0.0f, 0.0f, i*0.5f));
		_sceneGraph->AddRootSibling(LightManager::pointLights[i]);
	}
}

void MapEditor::StartMapEditor()
{
	_camera->position = glm::vec3(-5.0f, 3.0f, 0);
	_camera->horizontalAngle = PI / 2;
	_camera->verticalAngle = -0.4f;

	CalculateCameraAngles();
}

void MapEditor::Update()
{
	HandleInputs();

	_uiManager->UpdateUI(_selectedObject, _parentingObjects);
}

void MapEditor::Draw()
{
	_gameManager->FillShader(ShaderManager::textureShader);
	_gameManager->FillShader(ShaderManager::materialShader);

	_sceneGraph->DrawSceneGraphObjects();
}


void MapEditor::CalculateCameraAngles()
{
	glfwSetCursorPos(_window, _screenWidth / 2, _screenHeight / 2);

	_camera->horizontalAngle += _cMouseSpeed * float(_screenWidth / 2 - _mouseX);
	_camera->verticalAngle += _cMouseSpeed * float(_screenHeight / 2 - _mouseY);

	if (_camera->horizontalAngle > PI * 2) _camera->horizontalAngle = -PI * 2;
	else if (_camera->horizontalAngle < -PI * 2) _camera->horizontalAngle = PI * 2;

	if (_camera->verticalAngle > PI / 2) _camera->verticalAngle = PI / 2;
	else if (_camera->verticalAngle < -PI / 2) _camera->verticalAngle = -PI / 2;

	_camera->direction = glm::vec3(
		cos(_camera->verticalAngle) * sin(_camera->horizontalAngle),
		sin(_camera->verticalAngle),
		cos(_camera->verticalAngle) * cos(_camera->horizontalAngle)
	);

	_camera->right = glm::vec3(
		sin(_camera->horizontalAngle - 3.14f / 2.0f),
		0,
		cos(_camera->horizontalAngle - 3.14f / 2.0f)
	);

	_camera->up = glm::cross(_camera->right, _camera->direction);

	MatrixHolder::projectionMatrix = glm::perspective(_camera->FoV, 16.0f / 9.0f, 0.1f, 100.0f);

	MatrixHolder::viewMatrix = glm::lookAt(
		_camera->position,
		_camera->position + _camera->direction,
		_camera->up
	);
}

void MapEditor::HandleInputs()
{
	static bool mouseWarp = false;

	glfwGetCursorPos(_window, &_mouseX, &_mouseY);

	if (mouseWarp) {
		CalculateCameraAngles();
	}

	static bool f5Pressed = false;
	if (glfwGetKey(_window, GLFW_KEY_F5) == GLFW_PRESS && !f5Pressed)
	{
		_uiManager->SaveJson();
		_gameManager->SwitchGameState(GameState::GamePlayState);
		f5Pressed = true;
		return;
	}
	else if (glfwGetKey(_window, GLFW_KEY_F5) == GLFW_RELEASE && f5Pressed)
	{
		f5Pressed = false;
	}

	if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemActive()) {

		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
			_camera->position += _camera->direction * GameStateManager::deltaTime * _camera->speed;
		}
		if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
			_camera->position -= _camera->direction * GameStateManager::deltaTime * _camera->speed;
		}
		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
			_camera->position += _camera->right * GameStateManager::deltaTime * _camera->speed;
		}
		if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
			_camera->position -= _camera->right * GameStateManager::deltaTime * _camera->speed;
		}
		static bool deletePressed = false;
		if (glfwGetKey(_window, GLFW_KEY_DELETE) == GLFW_PRESS && !deletePressed)
		{
			if (_selectedObject != nullptr && (_selectedObject->GetType() != ObjectType::POINT_LIGHT && _selectedObject->GetType() != ObjectType::DIRECTIONAL_LIGHT && _selectedObject->GetType() != ObjectType::ROOT)) {
				_sceneGraph->EmptySingle(_selectedObject);
				_selectedObject = nullptr;
			}
			deletePressed = true;
		}
		else if (glfwGetKey(_window, GLFW_KEY_DELETE) == GLFW_RELEASE && deletePressed)
		{
			deletePressed = false;
		}


		static bool cPressed = false;
		if (glfwGetKey(_window, GLFW_KEY_C) == GLFW_PRESS && !cPressed) {
			mouseWarp = !mouseWarp;
			cPressed = true;
			glfwSetCursorPos(_window, _screenWidth / 2, _screenHeight / 2);
		}
		else if (glfwGetKey(_window, GLFW_KEY_C) == GLFW_RELEASE && cPressed) {
			cPressed = false;
		}

		static bool leftHeld = false;
		if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !leftHeld)
		{

			glm::vec3 rayOrigin;
			glm::vec3 rayDirection;

			glm::vec3 AABBMin;
			glm::vec3 AABBMax;

			float intersectionDistance = 1000;
			float tempDistance;
			// Alters rayOrigin and rayDirection relative to world space
			_camera->ScreenPositionToWorldRay((float)_mouseX, (float)_mouseY, _screenWidth, _screenHeight, MatrixHolder::viewMatrix, MatrixHolder::projectionMatrix, rayOrigin, rayDirection);

			vector<Node*> objectNodes;
			_sceneGraph->GetAllObjects(objectNodes);

			for each(Node* node in objectNodes)
			{
				switch (node->data->GetType())
				{
				case ObjectType::DYNAMIC_MATERIAL_OBJECT:
				case ObjectType::DYNAMIC_TEXTURED_OBJECT:
				case ObjectType::STATIC_MATERIAL_OBJECT:
				case ObjectType::STATIC_TEXTURED_OBJECT:
					SceneObject* sceneObject;
					sceneObject = static_cast<SceneObject*>(node->data);
					sceneObject->GetAABB(AABBMax, AABBMin);

					if (_camera->TestRayIntersection(rayOrigin, rayDirection, AABBMin, AABBMax, sceneObject->GetModelMatrix(), tempDistance))
					{
						if (tempDistance < intersectionDistance)
						{
							if (_parentingObjects && _selectedObject != node->data)
							{
								Node* selectedObjectNode;
								_sceneGraph->FindObject(_selectedObject, selectedObjectNode);
								_sceneGraph->MoveObjectTo(selectedObjectNode, node);
							}
							else {
								_selectedObject = node->data;
							}
							intersectionDistance = tempDistance;
						}
					}
					break;
				case ObjectType::DIRECTIONAL_LIGHT:
				case ObjectType::POINT_LIGHT:
					AABBMin = glm::vec3(-0.5f, -0.5f, -0.5f);
					AABBMax = glm::vec3(0.5f, 0.5f, 0.5f);

					if (_camera->TestRayIntersection(rayOrigin, rayDirection, AABBMin, AABBMax, node->data->GetModelMatrix(), tempDistance))
					{
						if (tempDistance < intersectionDistance)
						{
							_selectedObject = node->data;
							intersectionDistance = tempDistance;
						}
					}
					break;
				case ObjectType::ROOT:
					break;
				default:
					break;
				}
			}

			leftHeld = true;
			_parentingObjects = false;
		}
		else if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE && leftHeld) {
			leftHeld = false;
		}
	}
	else {
		mouseWarp = false;
	}
}