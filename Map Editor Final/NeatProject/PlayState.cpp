#include "PlayState.h"

#include "ImGui\imgui.h"

#include "GameManager.h"

PlayState::PlayState(GameManager* gameManager, GLFWwindow* window, Camera* camera) : _cMouseSpeed(0.005f)
{
	_gameManager = gameManager;
	_window = window;
	_camera = camera;

	_sceneGraph = new SceneGraph();

	_io = new JSONIOFileManager(_gameManager, _sceneGraph);

	glfwGetWindowSize(_window, &_screenWidth, &_screenHeight);
	glfwGetCursorPos(_window, &_mouseX, &_mouseY);
}


PlayState::~PlayState()
{
}

void PlayState::StartPlayState()
{
	_io->LoadJson(FileManager::currentFilePath.c_str());
	_camera->position = glm::vec3(0.0f);
	_camera->horizontalAngle = PI / 2;
	_camera->verticalAngle = 0;

	_score = 20;
	vector<Node*> temp;
	_sceneGraph->GetAllObjects(temp);
	_maxObjects = (int)(temp.size() - 6);
	_currentObjects = _maxObjects;

	glfwSetCursorPos(_window, _screenWidth / 2, _screenHeight / 2);
	CalculateCameraAngles();
}

void PlayState::CollisionCheck(SceneObject* object)
{
	glm::vec3 AABBmin;
	glm::vec3 AABBmax;

	bool xCollision = false, yCollision = false, zCollision = false;

	object->GetAABB(AABBmax, AABBmin);
	glm::vec4 newAABBmax = object->GetModelMatrix() * object->GetScaleMatrix() * glm::vec4(AABBmax, 1.0f);
	glm::vec4 newAABBmin = object->GetModelMatrix() * object->GetScaleMatrix() * glm::vec4(AABBmin, 1.0f);

	if (newAABBmax.x < newAABBmin.x)
	{
		float temp = newAABBmin.x;
		newAABBmin.x = newAABBmax.x;
		newAABBmax.x = temp;
	}
	if (newAABBmax.y < newAABBmin.y)
	{
		float temp = newAABBmin.y;
		newAABBmin.y = newAABBmax.y;
		newAABBmax.y = temp;
	}
	if (newAABBmax.z < newAABBmin.z)
	{
		float temp = newAABBmin.z;
		newAABBmin.z = newAABBmax.z;
		newAABBmax.z = temp;
	}

	if ((_cameraAABBmax.x >= newAABBmin.x) && (_cameraAABBmin.x <= newAABBmax.x)) xCollision = true;
	if ((_cameraAABBmax.y >= newAABBmin.y) && (_cameraAABBmin.y <= newAABBmax.y)) yCollision = true;
	if ((_cameraAABBmax.z >= newAABBmin.z) && (_cameraAABBmin.z <= newAABBmax.z)) zCollision = true;

	if (xCollision && yCollision && zCollision)
	{
		_sceneGraph->EmptySingle(object);
		_score += 10;
		_currentObjects--;
	}
}

void PlayState::Update()
{
	if (_currentObjects <= 0 || _score <= 0)
	{
		printf("Score: %f\n", _score);
		_gameManager->SwitchGameState(GameState::MapEditorState);
	}

	HandleInputs(GameStateManager::deltaTime);

	_cameraAABBmax = glm::vec3(0.5f) + _camera->position;
	_cameraAABBmin = glm::vec3(-0.5f) + _camera->position;

	_sceneGraph->UpdateSceneGraphObject();

	vector<Node*> objects;

	_sceneGraph->GetAllObjects(objects);

	for each (Node* objectNode in objects)
	{
		if (objectNode->data->GetType() == ObjectType::STATIC_MATERIAL_OBJECT || objectNode->data->GetType() == ObjectType::STATIC_TEXTURED_OBJECT || objectNode->data->GetType() == ObjectType::DYNAMIC_TEXTURED_OBJECT || objectNode->data->GetType() == ObjectType::DYNAMIC_MATERIAL_OBJECT) CollisionCheck(static_cast<SceneObject*>(objectNode->data));
	}

	_score -= GameStateManager::deltaTime;
}

void PlayState::Draw()
{
	_gameManager->FillShader(ShaderManager::textureShader);
	_gameManager->FillShader(ShaderManager::materialShader);

	_sceneGraph->DrawSceneGraphObjects();

	ImGui::GetIO().Fonts->Fonts[0]->Scale = 2.0f;
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin("Score", (bool*)true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::SetWindowPos(ImVec2((_screenWidth / 2.0f) - (ImGui::GetWindowWidth() /2.0f), 0.0f));
	ImGui::Text("Objects: %d / %d", _currentObjects, _maxObjects);
	ImGui::Text("Score: %.3f", _score);
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopFont();
	ImGui::GetIO().Fonts->Fonts[0]->Scale = 1.0f;
}

void PlayState::CalculateCameraAngles()
{
	glfwGetCursorPos(_window, &_mouseX, &_mouseY);
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

void PlayState::HandleInputs(float deltaTime)
{
	CalculateCameraAngles();

	static bool escapePressed = false;
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !escapePressed)
	{
		_gameManager->SwitchGameState(GameState::MapEditorState);
		escapePressed = true;
		return;
	}
	else if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_RELEASE && escapePressed)
	{
		escapePressed = false;
	}

	if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
		_camera->position += _camera->direction * deltaTime * _camera->speed;
	}
	if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
		_camera->position -= _camera->direction * deltaTime * _camera->speed;
	}
	if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
		_camera->position += _camera->right * deltaTime * _camera->speed;
	}
	if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
		_camera->position -= _camera->right * deltaTime * _camera->speed;
	}


}