#include "IOFileManager.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "rapidjson\ostreamwrapper.h"
#include "rapidjson\writer.h"
#include "ImGui/imgui.h"

#include "GameManager.h"

#include "DynamicTextured.h"
#include "StaticTextured.h"

#include "DirectionalLight.h"
#include "PointLight.h"


JSONIOFileManager::JSONIOFileManager(GameManager *gameManager, SceneGraph* sceneGraph)
{
	_sceneGraph = sceneGraph;
	_gameManager = gameManager;
}


JSONIOFileManager::~JSONIOFileManager()
{
}

Material* JSONIOFileManager::MaterialFromValue(Value& objectValue)
{
	Material* returnValue = new Material();
	returnValue->ambient.x = objectValue["Material"]["Ambient"][0].GetFloat();
	returnValue->ambient.y = objectValue["Material"]["Ambient"][1].GetFloat();
	returnValue->ambient.z = objectValue["Material"]["Ambient"][2].GetFloat();
	returnValue->diffuse.x = objectValue["Material"]["Diffuse"][0].GetFloat();
	returnValue->diffuse.y = objectValue["Material"]["Diffuse"][1].GetFloat();
	returnValue->diffuse.z = objectValue["Material"]["Diffuse"][2].GetFloat();
	returnValue->specular.x = objectValue["Material"]["Specular"][0].GetFloat();
	returnValue->specular.y = objectValue["Material"]["Specular"][1].GetFloat();
	returnValue->specular.z = objectValue["Material"]["Specular"][2].GetFloat();
	returnValue->shininess = objectValue["Material"]["Shininess"].GetFloat();

	return returnValue;
}
void JSONIOFileManager::ObjectPropertiesFromValue(Value& objectValue, glm::vec3 &position, string &name)
{
	position.x = objectValue["Position"][0].GetFloat();
	position.y = objectValue["Position"][1].GetFloat();
	position.z = objectValue["Position"][2].GetFloat();

	name = objectValue["Name"].GetString();
}
void JSONIOFileManager::SceneObjectPropertiesFromValue(Value& objectValue, glm::vec3 &rotation, glm::vec3 &scaleFactor)
{
	rotation.x = objectValue["Rotation"][0].GetFloat();
	rotation.y = objectValue["Rotation"][1].GetFloat();
	rotation.z = objectValue["Rotation"][2].GetFloat();
	scaleFactor.x = objectValue["Scale"][0].GetFloat();
	scaleFactor.y = objectValue["Scale"][1].GetFloat();
	scaleFactor.z = objectValue["Scale"][2].GetFloat();
}
void JSONIOFileManager::DynamicPropertiesFromValue(Value& objectValue, glm::vec3 &velocity, glm::vec3 &rotationVelocity)
{
	velocity.x = objectValue["Velocity"][0].GetFloat();
	velocity.y = objectValue["Velocity"][1].GetFloat();
	velocity.z = objectValue["Velocity"][2].GetFloat();

	rotationVelocity.x = objectValue["Rotation Velocity"][0].GetFloat();
	rotationVelocity.y = objectValue["Rotation Velocity"][1].GetFloat();
	rotationVelocity.z = objectValue["Rotation Velocity"][2].GetFloat();
}
DirLight* JSONIOFileManager::DirectionalLightFromValue(Value& objectValue)
{
	DirLight returnValue;
	returnValue.ambient.x = objectValue["Light Properties"]["Ambient"][0].GetFloat();
	returnValue.ambient.y = objectValue["Light Properties"]["Ambient"][1].GetFloat();
	returnValue.ambient.z = objectValue["Light Properties"]["Ambient"][2].GetFloat();
	returnValue.diffuse.x = objectValue["Light Properties"]["Diffuse"][0].GetFloat();
	returnValue.diffuse.y = objectValue["Light Properties"]["Diffuse"][1].GetFloat();
	returnValue.diffuse.z = objectValue["Light Properties"]["Diffuse"][2].GetFloat();
	returnValue.specular.x = objectValue["Light Properties"]["Specular"][0].GetFloat();
	returnValue.specular.y = objectValue["Light Properties"]["Specular"][1].GetFloat();
	returnValue.specular.z = objectValue["Light Properties"]["Specular"][2].GetFloat();

	return new DirLight(returnValue);
}
PntLight* JSONIOFileManager::PointLightFromValue(Value& objectValue)
{
	DirLight* temp = DirectionalLightFromValue(objectValue);
	PntLight returnValue;

	returnValue.ambient = temp->ambient;
	returnValue.diffuse = temp->diffuse;
	returnValue.specular = temp->specular;

	returnValue.constant = objectValue["Light Properties"]["Constant"].GetFloat();
	returnValue.linear = objectValue["Light Properties"]["Linear"].GetFloat();
	returnValue.quadratic = objectValue["Light Properties"]["Quadratic"].GetFloat();

	return new PntLight(returnValue);
}

Object* JSONIOFileManager::ConvertJsonToObject(Node* parentNode, Value& objectValue)
{
	VBO* mesh = nullptr;
	Texture2D* texture = nullptr, *textureSpecular = nullptr;
	if (objectValue.HasMember("Mesh Path")) mesh = _gameManager->LoadMesh(objectValue["Mesh Path"].GetString());
	if (objectValue.HasMember("Texture Path"))
	{
		texture = _gameManager->LoadTexture(objectValue["Texture Path"].GetString());
		textureSpecular = _gameManager->LoadSpecular(objectValue["Texture Path"].GetString());
	}
	glm::vec3 position;
	string name;

	glm::vec3 rotation;
	glm::vec3 velocity;
	glm::vec3 rotationVelocity;

	glm::vec3 scaleFactor;

	ObjectPropertiesFromValue(objectValue, position, name);

	switch (objectValue["Type"].GetInt())
	{
	case ObjectType::DYNAMIC_MATERIAL_OBJECT:
		SceneObjectPropertiesFromValue(objectValue, rotation, scaleFactor);
		DynamicPropertiesFromValue(objectValue, velocity, rotationVelocity);
		DynamicObject* dynaMat;
		dynaMat = new DynamicObject(mesh, MaterialFromValue(objectValue), position);
		dynaMat->SetName(name);
		dynaMat->SetRotationAngle(rotation);
		dynaMat->SetRotationVelocity(rotationVelocity);
		dynaMat->SetVelocity(velocity);
		dynaMat->SetScaleFactor(scaleFactor);
		return dynaMat;
		break;
	case ObjectType::DYNAMIC_TEXTURED_OBJECT:
		SceneObjectPropertiesFromValue(objectValue, rotation, scaleFactor);
		DynamicPropertiesFromValue(objectValue, velocity, rotationVelocity);
		DynamicTextured* dynaText;
		dynaText = new DynamicTextured(new DynamicObject(mesh, MaterialFromValue(objectValue), position), texture, textureSpecular);
		dynaText->SetName(name);
		dynaText->SetRotationAngle(rotation);
		dynaText->SetRotationVelocity(rotationVelocity);
		dynaText->SetVelocity(velocity);
		dynaText->SetScaleFactor(scaleFactor);
		return dynaText;
		break;
	case ObjectType::STATIC_MATERIAL_OBJECT:
		SceneObjectPropertiesFromValue(objectValue, rotation, scaleFactor);
		StaticObject* staticMat;
		staticMat = new StaticObject(mesh, MaterialFromValue(objectValue), position);
		staticMat->SetName(name);
		staticMat->SetRotationAngle(rotation);
		staticMat->SetScaleFactor(scaleFactor);
		return staticMat;
		break;
	case ObjectType::STATIC_TEXTURED_OBJECT:
		SceneObjectPropertiesFromValue(objectValue, rotation, scaleFactor);
		StaticTextured* staticTex;
		staticTex = new StaticTextured(new StaticObject(mesh, MaterialFromValue(objectValue), position), texture, textureSpecular);
		staticTex->SetName(name);
		staticTex->SetRotationAngle(rotation);
		staticTex->SetScaleFactor(scaleFactor);
		return staticTex;
		break;
	case ObjectType::DIRECTIONAL_LIGHT:
		DirectionalLight* directionalLight;
		directionalLight = new DirectionalLight(_gameManager->LoadTexture("2D/sunBillboard.tga"), DirectionalLightFromValue(objectValue), position);
		LightManager::directionalLight = directionalLight;
		return directionalLight;
		break;
	case ObjectType::POINT_LIGHT:
		PointLight* pointLight;
		pointLight =  new PointLight(_gameManager->LoadTexture("2D/lightBillboard.tga"), PointLightFromValue(objectValue), position);
		LightManager::pointLights[_lightCount] = pointLight;
		_lightCount++;
		return pointLight;
		break;
	default:
		break;
	}
	return nullptr;
}

void JSONIOFileManager::GetChildren(Node* parentNode, Value& childrenArray)
{
	if (childrenArray.IsArray() && childrenArray.Size() > 0)
	{	
		for (Value& child : childrenArray.GetArray())
		{
			Node* objectNode = _sceneGraph->AddChild(ConvertJsonToObject(parentNode, child), parentNode);
			GetChildren(objectNode, child["Children"]);
		}
	}
	return;
}

void JSONIOFileManager::GrabDataFromJson(std::string jsonData)
{
	_openedDocument.Parse(jsonData.c_str());
	if (_openedDocument.HasMember("Scene Graph") && _openedDocument["Scene Graph"].HasMember("Children"))
	{
		_lightCount = 0;
		GetChildren(_sceneGraph->GetRoot(), _openedDocument["Scene Graph"]["Children"]);
	}
	else {
		printf("Map file cannot be loaded due to format issues.");
	}
}

void JSONIOFileManager::LoadJson(const char* path)
{
	std::string jsonDump;
	std::string sPath("saves/maps/" + string(path) + ".json");
	std::ifstream file;
	file.open(sPath, std::ios::in);

	if (!file.good())
	{
		printf("Error: Can't open json file");
		return;
	}

	FileManager::currentFilePath = string(path);

	_sceneGraph->EmptyTree();

	std::string line;

	while (getline(file, line))
	{
		jsonDump += line;
	}
	
	GrabDataFromJson(jsonDump);
}

void JSONIOFileManager::SceneObjectFromSceneGraph(Node* node, Value &returnValue)
{
	SceneObject* object = static_cast<SceneObject*>(node->data);

	Value meshPath(StringRef((*object->GetVertexBufferObject()->path).c_str()));
	returnValue.AddMember("Mesh Path", meshPath, _openedDocument.GetAllocator());

	Value material(kObjectType);
	Value ambient(kArrayType);
	ambient.PushBack(object->GetMaterial()->ambient.x, _openedDocument.GetAllocator());
	ambient.PushBack(object->GetMaterial()->ambient.y, _openedDocument.GetAllocator());
	ambient.PushBack(object->GetMaterial()->ambient.z, _openedDocument.GetAllocator());
	material.AddMember("Ambient", ambient, _openedDocument.GetAllocator());
	Value diffuse(kArrayType);
	diffuse.PushBack(object->GetMaterial()->diffuse.x, _openedDocument.GetAllocator());
	diffuse.PushBack(object->GetMaterial()->diffuse.y, _openedDocument.GetAllocator());
	diffuse.PushBack(object->GetMaterial()->diffuse.z, _openedDocument.GetAllocator());
	material.AddMember("Diffuse", diffuse, _openedDocument.GetAllocator());
	Value specular(kArrayType);
	specular.PushBack(object->GetMaterial()->specular.x, _openedDocument.GetAllocator());
	specular.PushBack(object->GetMaterial()->specular.y, _openedDocument.GetAllocator());
	specular.PushBack(object->GetMaterial()->specular.z, _openedDocument.GetAllocator());
	material.AddMember("Specular", specular, _openedDocument.GetAllocator());
	Value shininess(object->GetMaterial()->shininess);
	material.AddMember("Shininess", shininess, _openedDocument.GetAllocator());
	returnValue.AddMember("Material", material, _openedDocument.GetAllocator());

	Value rotation(kArrayType);
	rotation.PushBack(object->GetRawRotationAngle().x, _openedDocument.GetAllocator());
	rotation.PushBack(object->GetRawRotationAngle().y, _openedDocument.GetAllocator());
	rotation.PushBack(object->GetRawRotationAngle().z, _openedDocument.GetAllocator());
	returnValue.AddMember("Rotation", rotation, _openedDocument.GetAllocator());

	Value scaleFactor(kArrayType);
	scaleFactor.PushBack(object->GetScaleFactor().x, _openedDocument.GetAllocator());
	scaleFactor.PushBack(object->GetScaleFactor().y, _openedDocument.GetAllocator());
	scaleFactor.PushBack(object->GetScaleFactor().z, _openedDocument.GetAllocator());
	returnValue.AddMember("Scale", scaleFactor, _openedDocument.GetAllocator());
}

void JSONIOFileManager::DynamicObjectFromSceneGraph(Node* node, Value &returnValue)
{
	SceneObjectFromSceneGraph(node, returnValue);

	DynamicObject* object = static_cast<DynamicObject*>(node->data);
	Value velocity(kArrayType);
	velocity.PushBack(object->GetVelocity().x, _openedDocument.GetAllocator());
	velocity.PushBack(object->GetVelocity().y, _openedDocument.GetAllocator());
	velocity.PushBack(object->GetVelocity().z, _openedDocument.GetAllocator());
	returnValue.AddMember("Velocity", velocity, _openedDocument.GetAllocator());

	Value rotationVelocity(kArrayType);
	rotationVelocity.PushBack(object->GetRawRotationVelocity().x, _openedDocument.GetAllocator());
	rotationVelocity.PushBack(object->GetRawRotationVelocity().y, _openedDocument.GetAllocator());
	rotationVelocity.PushBack(object->GetRawRotationVelocity().z, _openedDocument.GetAllocator());
	returnValue.AddMember("Rotation Velocity", rotationVelocity, _openedDocument.GetAllocator());
}
void JSONIOFileManager::DynamicTexturedFromSceneGraph(Node* node, Value &returnValue)
{
	DynamicObjectFromSceneGraph(node, returnValue);

	DynamicTextured* object = static_cast<DynamicTextured*>(node->data);
	Value texturePath(StringRef((*object->GetTexture()->GetTexturePath()).c_str()));
	returnValue.AddMember("Texture Path", texturePath, _openedDocument.GetAllocator());
}
void JSONIOFileManager::StaticObjectFromSceneGraph(Node* node, Value &returnValue)
{
	SceneObjectFromSceneGraph(node, returnValue);
}
void JSONIOFileManager::StaticTexturedFromSceneGraph(Node* node, Value &returnValue)
{
	StaticObjectFromSceneGraph(node, returnValue);

	StaticTextured* object = static_cast<StaticTextured*>(node->data);
	Value texturePath(StringRef((*object->GetTexture()->GetTexturePath()).c_str()));
	returnValue.AddMember("Texture Path", texturePath, _openedDocument.GetAllocator());
}
void JSONIOFileManager::DirectionalLightFromSceneGraph(Node* node, Value &returnValue)
{
	DirectionalLight* object = static_cast<DirectionalLight*>(node->data);

	Value lightProperties(kObjectType);
	Value ambient(kArrayType);
	ambient.PushBack(object->GetLightParameters()->ambient.x, _openedDocument.GetAllocator());
	ambient.PushBack(object->GetLightParameters()->ambient.y, _openedDocument.GetAllocator());
	ambient.PushBack(object->GetLightParameters()->ambient.z, _openedDocument.GetAllocator());
	lightProperties.AddMember("Ambient", ambient, _openedDocument.GetAllocator());
	Value diffuse(kArrayType);
	diffuse.PushBack(object->GetLightParameters()->diffuse.x, _openedDocument.GetAllocator());
	diffuse.PushBack(object->GetLightParameters()->diffuse.y, _openedDocument.GetAllocator());
	diffuse.PushBack(object->GetLightParameters()->diffuse.z, _openedDocument.GetAllocator());
	lightProperties.AddMember("Diffuse", diffuse, _openedDocument.GetAllocator());
	Value specular(kArrayType);
	specular.PushBack(object->GetLightParameters()->specular.x, _openedDocument.GetAllocator());
	specular.PushBack(object->GetLightParameters()->specular.y, _openedDocument.GetAllocator());
	specular.PushBack(object->GetLightParameters()->specular.z, _openedDocument.GetAllocator());
	lightProperties.AddMember("Specular", specular, _openedDocument.GetAllocator());
	returnValue.AddMember("Light Properties", lightProperties, _openedDocument.GetAllocator());
}
void JSONIOFileManager::PointLightFromSceneGraph(Node* node, Value &returnValue)
{
	DirectionalLightFromSceneGraph(node, returnValue);

	PointLight* object = static_cast<PointLight*>(node->data);

	returnValue["Light Properties"].AddMember("Constant", object->GetLightParameters()->constant, _openedDocument.GetAllocator());
	returnValue["Light Properties"].AddMember("Linear", object->GetLightParameters()->linear, _openedDocument.GetAllocator());
	returnValue["Light Properties"].AddMember("Quadratic", object->GetLightParameters()->quadratic, _openedDocument.GetAllocator());
}

Value JSONIOFileManager::ExtractJSONFromSceneGraph(Node* node)
{
	Value returnValue(kObjectType);

	Value name (StringRef((*node->data->GetName()).c_str()));
	returnValue.AddMember("Name", name, _openedDocument.GetAllocator());

	Value type(node->data->GetType());
	returnValue.AddMember("Type", type, _openedDocument.GetAllocator());

	Value position(kArrayType);
	position.PushBack(node->data->GetModelPosition().x, _openedDocument.GetAllocator());
	position.PushBack(node->data->GetModelPosition().y, _openedDocument.GetAllocator());
	position.PushBack(node->data->GetModelPosition().z, _openedDocument.GetAllocator());
	returnValue.AddMember("Position", position, _openedDocument.GetAllocator());

	switch (node->data->GetType())
	{
	case ObjectType::DIRECTIONAL_LIGHT:
		DirectionalLightFromSceneGraph(node, returnValue);
		break;
	case ObjectType::POINT_LIGHT:
		PointLightFromSceneGraph(node, returnValue);
		break;
	case ObjectType::DYNAMIC_MATERIAL_OBJECT:
		DynamicObjectFromSceneGraph(node, returnValue);
		break;
	case ObjectType::DYNAMIC_TEXTURED_OBJECT:
		DynamicTexturedFromSceneGraph(node, returnValue);
		break;
	case ObjectType::STATIC_MATERIAL_OBJECT:
		StaticObjectFromSceneGraph(node, returnValue);
		break;
	case ObjectType::STATIC_TEXTURED_OBJECT:
		StaticTexturedFromSceneGraph(node, returnValue);
		break;
	default:
		break;
	}

	Value childrenArray(kArrayType);
	vector<Node*> children;
	_sceneGraph->GetAllChildren(node, children);
	for each(Node* child in children)
	{
		childrenArray.PushBack(ExtractJSONFromSceneGraph(child), _openedDocument.GetAllocator());
	}
	returnValue.AddMember("Children", childrenArray, _openedDocument.GetAllocator());

	return returnValue;
}

void JSONIOFileManager::ConstructJson()
{
	Node* root = _sceneGraph->GetRoot();
	_openedDocument.SetObject();

	Value sceneGraphArray(kArrayType);
	sceneGraphArray = ExtractJSONFromSceneGraph(root);

	_openedDocument.AddMember("Scene Graph", sceneGraphArray, _openedDocument.GetAllocator());
}

void JSONIOFileManager::SaveJson(const char* path)
{
	ConstructJson();

	std::string sPath = "saves/maps/" + string(path) + ".json";

	std::ofstream output(sPath);

	if (!output.good())
	{
		printf("Save error\n");
		return;
	}

	FileManager::currentFilePath = string(path);

	OStreamWrapper streamWrapper(output);

	Writer<OStreamWrapper> writer(streamWrapper);
	_openedDocument.Accept(writer);

	output.close();
}

void JSONIOFileManager::SaveJson()
{
	SaveJson(FileManager::currentFilePath.c_str());
}