#pragma once

#include "rapidjson\document.h"
#include "SceneGraph.h"

class GameManager;

using namespace rapidjson;

class JSONIOFileManager
{
private:
	Document _openedDocument;
	SceneGraph* _sceneGraph;
	GameManager* _gameManager;

	int _lightCount;

	Material* MaterialFromValue(Value& objectValue);
	void ObjectPropertiesFromValue(Value& objectValue, glm::vec3 &position, string &name);
	void SceneObjectPropertiesFromValue(Value& objectValue, glm::vec3 &rotation, glm::vec3 &scaleFactor);
	void DynamicPropertiesFromValue(Value& objectValue, glm::vec3 &velocity, glm::vec3 &rotationVelocity);
	DirLight* DirectionalLightFromValue(Value& objectValue);
	PntLight* PointLightFromValue(Value& objectValue);

	Object* ConvertJsonToObject(Node* parentNode, Value& objectValue);

	void GetChildren(Node* parentNode, Value& childrenArray);

	void GrabDataFromJson(std::string jsonData);

	void SceneObjectFromSceneGraph(Node* node, Value &returnValue);
	void DynamicObjectFromSceneGraph(Node* node, Value &returnValue);
	void DynamicTexturedFromSceneGraph(Node* node, Value &returnValue);
	void StaticObjectFromSceneGraph(Node* node, Value &returnValue);
	void StaticTexturedFromSceneGraph(Node* node, Value &returnValue);
	void DirectionalLightFromSceneGraph(Node* node, Value &returnValue);
	void PointLightFromSceneGraph(Node* node, Value &returnValue);

	Value ExtractJSONFromSceneGraph(Node* node);

	void ConstructJson();
public:
	JSONIOFileManager(GameManager *gameManager, SceneGraph* _sceneGraph);
	~JSONIOFileManager();

	void LoadJson(const char* path);

	void SaveJson(const char* path);
	void SaveJson();
};

