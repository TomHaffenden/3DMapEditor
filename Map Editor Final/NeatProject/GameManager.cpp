#include "GameManager.h"

#include <string>
#include <fstream>

#include <glm\gtc\matrix_transform.hpp>
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_glfw_gl3.h"

#include "common\Loader.h"
#include "common\Structures.h"
#include "common\Texture2D.h"

GameManager::GameManager()
{
	_screenWidth = 1280;
	_screenHeight = 72;

	int ret = InitOpenGL();
	if (ret != 0) exit(ret);
	ImGui_ImplGlfwGL3_Init(_window, true);
	LoadResources();

	double lastTime = glfwGetTime();
	do {
		double currentTime = glfwGetTime();

		if (currentTime - lastTime >= 0.00833f)
		{
			Update();

			Draw();

			lastTime = currentTime;
		}

	}while (glfwWindowShouldClose(_window) == 0);
	glDeleteProgram(ShaderManager::textureShader.shaderID);
	glDeleteProgram(ShaderManager::materialShader.shaderID);
	glDeleteProgram(ShaderManager::billboardShader.shaderID);
	glDeleteVertexArrays(1, &_vertexArrayID);
	glfwTerminate();
}


GameManager::~GameManager()
{
	glfwDestroyWindow(_window);
	_window = nullptr;
	delete _camera;
	_camera = nullptr;
}

int GameManager::InitOpenGL()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(_screenWidth, _screenHeight, "Tom's OpenGL 3.3", NULL, NULL);
	if (_window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(_window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(_window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(_window, _screenWidth / 2.0f, _screenHeight / 2.0f);
	glfwPollEvents();

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	_vertexArrayID;
	glGenVertexArrays(1, &_vertexArrayID);
	glBindVertexArray(_vertexArrayID);

	ShaderManager::textureShader.shaderID = Loader::LoadShaders("shaders/ObjectShading.vertexshader", "shaders/ObjectShading.fragmentshader");
	ShaderManager::billboardShader.shaderID = Loader::LoadShaders("shaders/BillboardShading.vertexshader", "shaders/BillboardShading.fragmentshader");
	ShaderManager::materialShader.shaderID = Loader::LoadShaders("shaders/MaterialShading.vertexshader", "shaders/MaterialShading.fragmentshader");

	return 0;
}

void GameManager::LoadResources()
{
	_textures["default"] = new Texture2D();
	_textures["default"]->LoadBMP("materials/textures/DefaultTexture.bmp");
	_textures["default"]->SetTextureSampler(ShaderManager::textureShader.shaderID, "material.diffuse");
	_textures["default"]->GetTexturePath()->assign("default");

	_speculars["default"] = new Texture2D();
	_speculars["default"]->LoadBMP("materials/textures/DefaultSpecular.bmp");
	_speculars["default"]->SetTextureSampler(ShaderManager::textureShader.shaderID, "material.specular");

	_meshes["default"] = Loader::loadOBJ("models/cube.obj");
	_meshes["default"]->path = new string("default");

	_camera = new Camera(glm::vec3(-5.0f, 3.0f, 0), 70.0f, 5.0f);

	_textures["2D/sunBillboard.tga"] = new Texture2D();
	_textures["2D/sunBillboard.tga"]->LoadTGA("materials/textures/2D/sunBillboard.tga");
	_textures["2D/sunBillboard.tga"]->SetTextureSampler(ShaderManager::billboardShader.shaderID, "billboardTextureSampler");
	_textures["2D/sunBillboard.tga"]->GetTexturePath()->assign("2D/sunBillboard.tga");

	_textures["2D/lightBillboard.tga"] = new Texture2D();
	_textures["2D/lightBillboard.tga"]->LoadTGA("materials/textures/2D/lightBillboard.tga");
	_textures["2D/lightBillboard.tga"]->SetTextureSampler(ShaderManager::billboardShader.shaderID, "billboardTextureSampler");
	_textures["2D/lightBillboard.tga"]->GetTexturePath()->assign("2D/lightBillboard.tga");
	
	LoadShader(ShaderManager::textureShader);

	LoadShader(ShaderManager::materialShader);

	_sceneGraph = new SceneGraph();

	_uiManager = new UIManager(this, _sceneGraph);

	_mapEditor = new MapEditor(this, _window, _uiManager, _sceneGraph, _camera);
	_playState = new PlayState(this, _window, _camera);
}

void GameManager::SwitchGameState(GameState gameState)
{
	switch (gameState)
	{
	case GameState::MapEditorState:
		_mapEditor->StartMapEditor();
		GameStateManager::gameState = GameState::MapEditorState;
		break;
	case GameState::GamePlayState:
		_playState->StartPlayState();
		GameStateManager::gameState = GameState::GamePlayState;
		break;
	default:
		break;
	}
}

void GameManager::LoadNewMap()
{
	_mapEditor->LoadNewMap();
}

void GameManager::LoadShader(LightingShader& shader)
{
	shader.viewPosID = glGetUniformLocation(shader.shaderID, "viewPos");
	shader.matrixID = glGetUniformLocation(shader.shaderID, "MVP");
	shader.viewMatrixID = glGetUniformLocation(shader.shaderID, "V");

	shader.dirLightPositionID = glGetUniformLocation(shader.shaderID, "dirLight.position");
	shader.dirLightAmbientID = glGetUniformLocation(shader.shaderID, "dirLight.ambient");
	shader.dirLightDiffuseID = glGetUniformLocation(shader.shaderID, "dirLight.diffuse");
	shader.dirLightSpecularID = glGetUniformLocation(shader.shaderID, "dirLight.specular");

	for (int i = 0; i < 4; i++)
	{

		string locationPath = "pointLights[" + std::to_string(i) + string("].position");
		shader.pointLightPositionID[i] = glGetUniformLocation(shader.shaderID, locationPath.c_str());
		locationPath = "pointLights[" + std::to_string(i) + string("].ambient");
		shader.pointLightAmbientID[i] = glGetUniformLocation(shader.shaderID, locationPath.c_str());
		locationPath = "pointLights[" + std::to_string(i) + string("].diffuse");
		shader.pointLightDiffuseID[i] = glGetUniformLocation(shader.shaderID, locationPath.c_str());
		locationPath = "pointLights[" + std::to_string(i) + string("].specular");
		shader.pointLightSpecularID[i] = glGetUniformLocation(shader.shaderID, locationPath.c_str());

		locationPath = "pointLights[" + std::to_string(i) + string("].constant");
		shader.pointLightConstantID[i] = glGetUniformLocation(shader.shaderID, locationPath.c_str());
		locationPath = "pointLights[" + std::to_string(i) + string("].linear");
		shader.pointLightLinearID[i] = glGetUniformLocation(shader.shaderID, locationPath.c_str());
		locationPath = "pointLights[" + std::to_string(i) + string("].quadratic");
		shader.pointLightQuardraticID[i] = glGetUniformLocation(shader.shaderID, locationPath.c_str());
	}
}

Texture2D* GameManager::LoadTexture(string path)
{
	if (path == "")
	{
		return _textures["default"];
	}
	else if (_textures[path] == nullptr) {
		Texture2D* texture = new Texture2D();
		string sPath = "materials/textures/";
		sPath += path;
		if (path.substr(path.length() - 4, 4).compare(".bmp") == 0) {
			if (!texture->LoadBMP(sPath.c_str())) {
				printf(" Using default texture (failed bmp load)\n");
				return _textures["default"];
			}
		}
		else if (path.substr(path.length() - 4, 4).compare(".tga") == 0) {
			if (!texture->LoadTGA(sPath.c_str())) {
				printf(" Using default texture (faliled tga load)\n");
				return _textures["default"];
			}
		}
		else {
			printf("Using default texture (.tga and .bmp supported)\n");
			return _textures["default"];
		}
		texture->SetTextureSampler(ShaderManager::textureShader.shaderID, "material.diffuse");
		texture->GetTexturePath()->assign(path);
		_textures[path] = texture;
		return _textures[path];
	}
	else {
		return _textures[path];
	}
}

Texture2D* GameManager::LoadSpecular(string path)
{
	if (path.length() <= 4)
	{
		return _speculars["default"];
	}else if (_speculars[path] == nullptr) {
		Texture2D* texture = new Texture2D();
		string sPath = "materials/textures/";
		
		sPath += path.substr(0, path.length() - 4);
		sPath += "_specular";
		sPath += path.substr(path.length() - 4, 4);
		if (!texture->LoadBMP(sPath.c_str())) {
			printf(" Using default specular (failed to load bmp)\n");
			return _speculars["default"];
		}
		texture->SetTextureSampler(ShaderManager::textureShader.shaderID, "material.specular");
		texture->GetTexturePath()->assign(path);
		_speculars[path] = texture;
		return _speculars[path];
	}
	else {
		return _speculars[path];
	}
}

VBO* GameManager::LoadMesh(string path)
{
	if (_meshes[path] == nullptr)
	{
		string sPath = "models/";
		sPath += path;
		sPath += ".obj";
		VBO* temp = Loader::loadOBJ(sPath.c_str());
		if (!temp) {
			return _meshes["default"];
		}
		temp->path = new string(path.c_str());
		_meshes[path] = temp;
		return _meshes[path];
	}
	else {
		return _meshes[path];
	}
}

void GameManager::Update()
{
	static double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	GameStateManager::deltaTime = float(currentTime - lastTime);
	ImGui_ImplGlfwGL3_NewFrame();
	switch (GameStateManager::gameState) {
	case GameState::MapEditorState:
		_mapEditor->Update();
		break;
	case GameState::GamePlayState:
		_playState->Update();
		break;
	default:
		break;
	}
	lastTime = currentTime;
}

void GameManager::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	switch (GameStateManager::gameState) {
	case GameState::MapEditorState:
		_mapEditor->Draw();
		break;
	case GameState::GamePlayState:
		_playState->Draw();
		break;
	default:
		break;
	}

	ImGui::Render();

	glfwSwapBuffers(_window);
	glfwPollEvents();
}

void GameManager::FillShader(LightingShader& shader)
{
	glUseProgram(shader.shaderID);

	glUniform3f(shader.viewPosID, _camera->position.x, _camera->position.y, _camera->position.z);

	glUniform3f(shader.dirLightPositionID, LightManager::directionalLight->GetModelPosition().x, LightManager::directionalLight->GetModelPosition().y, LightManager::directionalLight->GetModelPosition().z);
	DirLight* dirLight = LightManager::directionalLight->GetLightParameters();
	glUniform3f(shader.dirLightAmbientID, dirLight->ambient.x, dirLight->ambient.y, dirLight->ambient.z);
	glUniform3f(shader.dirLightDiffuseID, dirLight->diffuse.x, dirLight->diffuse.y, dirLight->diffuse.z);
	glUniform3f(shader.dirLightSpecularID, dirLight->specular.x, dirLight->specular.y, dirLight->specular.z);

	for (int i = 0; i < 4; i++)
	{
		glm::vec4 lightPosition = LightManager::pointLights[i]->GetParentMatrix() * glm::vec4(LightManager::pointLights[i]->GetModelPosition(), 1.0f);

		glUniform3f(shader.pointLightPositionID[i], lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(shader.pointLightAmbientID[i], LightManager::pointLights[i]->GetLightParameters()->ambient.x, LightManager::pointLights[i]->GetLightParameters()->ambient.y, LightManager::pointLights[i]->GetLightParameters()->ambient.z);
		glUniform3f(shader.pointLightDiffuseID[i], LightManager::pointLights[i]->GetLightParameters()->diffuse.x, LightManager::pointLights[i]->GetLightParameters()->diffuse.y, LightManager::pointLights[i]->GetLightParameters()->diffuse.z);
		glUniform3f(shader.pointLightSpecularID[i], LightManager::pointLights[i]->GetLightParameters()->specular.x, LightManager::pointLights[i]->GetLightParameters()->specular.y, LightManager::pointLights[i]->GetLightParameters()->specular.z);

		glUniform1f(shader.pointLightConstantID[i], LightManager::pointLights[i]->GetLightParameters()->constant);
		glUniform1f(shader.pointLightLinearID[i], LightManager::pointLights[i]->GetLightParameters()->linear);
		glUniform1f(shader.pointLightQuardraticID[i], LightManager::pointLights[i]->GetLightParameters()->quadratic);
	}

	glUniformMatrix4fv(shader.viewMatrixID, 1, GL_FALSE, &MatrixHolder::viewMatrix[0][0]);
}