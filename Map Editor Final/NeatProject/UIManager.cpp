#include "UIManager.h"

#include "GameManager.h"

#include "DynamicTextured.h"
#include "StaticTextured.h"

#include <fstream>

UIManager::UIManager(GameManager* gameManager, SceneGraph* sceneGraph)
{
	_gameManager = gameManager;
	_sceneGraph = sceneGraph;
	_jsonIo = new JSONIOFileManager(gameManager, _sceneGraph);

	_popupType = (PopupType)-1;

	_sceneGraphOpen = true;
	_objectPropertiesOpen = true;
	_createObjectOpen = true;
}


UIManager::~UIManager()
{
}

void UIManager::DynamicTextureProperties(Object*& currentObject, bool &parenting)
{
	string sPath;
	static char texturePath[32];
	static bool editName = false;

	DynamicTextured* dynamicTexturedobject = static_cast<DynamicTextured*>(currentObject);
	glm::vec3 position = dynamicTexturedobject->GetModelPosition();
	glm::vec3 rotationAngle = dynamicTexturedobject->GetRotationAngle();
	glm::vec3 scaleFactor = dynamicTexturedobject->GetScaleFactor();
	glm::vec3 rotationVelocity = dynamicTexturedobject->GetRotationVelocity();
	glm::vec3 velocity = dynamicTexturedobject->GetVelocity();

	if (editName) {
		char* data;
		data = new char[128];
		sprintf(data, "%s", dynamicTexturedobject->GetName()->c_str());
		if (ImGui::InputText("Name", data, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
			dynamicTexturedobject->SetName(string(data));
			editName = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) editName = false;
	}
	else {
		ImGui::Text("Name: %s", dynamicTexturedobject->GetName()->c_str());
		ImGui::SameLine();
		if (ImGui::Button("Edit")) editName = true;
	}

	if (ImGui::DragFloat3("position", &position[0], 0.1f, -1000.0f, 1000.0f, "%.3f")) dynamicTexturedobject->SetModelPosition(position);
	if (ImGui::DragFloat3("rotation", &rotationAngle[0], 0.1f, -180, 180)) dynamicTexturedobject->SetRotationAngle(rotationAngle[0], rotationAngle[1], rotationAngle[2]);
	if (ImGui::DragFloat3("scale", &scaleFactor[0], 0.01f, 0.001f, 100.0f)) dynamicTexturedobject->SetScaleFactor(scaleFactor);
	if (ImGui::DragFloat3("rotation velocity", &rotationVelocity[0], 0.1f, -360, 360))	dynamicTexturedobject->SetRotationVelocity(rotationVelocity.x, rotationVelocity.y, rotationVelocity.z);
	if (ImGui::DragFloat3("velocity", &velocity[0], 0.001f, -2, 2)) dynamicTexturedobject->SetVelocity(velocity);


	ImGui::DragFloat("Shininess: ", &dynamicTexturedobject->GetMaterial()->shininess, 0.01f, 1.0f, 256.0f);
	ImGui::InputText("Texture path: ", texturePath, sizeof(texturePath));
	sPath = texturePath;

	if (ImGui::Button("Change texture")) {
		dynamicTexturedobject->ChangeTexture(_gameManager->LoadTexture(sPath), _gameManager->LoadTexture(sPath));
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Object"))
	{
		_sceneGraph->EmptySingle(currentObject);
	}

	if (ImGui::Button("Parent Object"))
	{
		parenting = true;
		printf("Parenting");
	}
	ImGui::SameLine();
	if (ImGui::Button("Unparent Object"))
	{
		Node* root = _sceneGraph->GetRoot();
		Node* object;
		_sceneGraph->FindObject(currentObject, object);
		_sceneGraph->MoveObjectTo(object, root);
	}
}
void UIManager::DynamicMaterialProperties(Object*& currentObject, bool &parenting)
{
	string sPath;
	static char texturePath[32];
	static bool editName = false;

	DynamicObject* dynamicObject = static_cast<DynamicObject*>(currentObject);
	glm::vec3 position = dynamicObject->GetModelPosition();
	glm::vec3 rotationAngle = dynamicObject->GetRotationAngle();
	glm::vec3 rotationVelocity = dynamicObject->GetRotationVelocity();
	glm::vec3 velocity = dynamicObject->GetVelocity();
	glm::vec3 scaleFactor = dynamicObject->GetScaleFactor();

	if (editName) {
		char* data;
		data = new char[128];
		sprintf(data, "%s", dynamicObject->GetName()->c_str());
		if (ImGui::InputText("Name", data, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
			dynamicObject->SetName(string(data));
			editName = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) editName = false;
	}
	else {
		ImGui::Text("Name: %s", dynamicObject->GetName()->c_str());
		ImGui::SameLine();
		if (ImGui::Button("Edit")) editName = true;
	}


	if (ImGui::DragFloat3("position", &position[0], 0.1f, -1000.0f, 1000.0f, "%.3f")) dynamicObject->SetModelPosition(position);
	if (ImGui::DragFloat3("rotation", &rotationAngle[0], 0.1f, -180, 180)) dynamicObject->SetRotationAngle(rotationAngle[0], rotationAngle[1], rotationAngle[2]);
	if (ImGui::DragFloat3("scale", &scaleFactor[0], 0.01f, 0.001f, 100.0f)) dynamicObject->SetScaleFactor(scaleFactor);
	if (ImGui::DragFloat3("rotation velocity", &rotationVelocity[0], 1.0f, -360, 360)) dynamicObject->SetRotationVelocity(rotationVelocity.x, rotationVelocity.y, rotationVelocity.z);
	if (ImGui::DragFloat3("velocity", &velocity[0], 0.001f, -2, 2)) dynamicObject->SetVelocity(velocity);

	ImGui::DragFloat3("ambient", &dynamicObject->GetMaterial()->ambient[0], 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat3("diffuse", &dynamicObject->GetMaterial()->diffuse[0], 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat3("specular", &dynamicObject->GetMaterial()->specular[0], 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat("Shininess", &dynamicObject->GetMaterial()->shininess, 0.01f, 1.0f, 256.0f);



	if (dynamicObject->GetVertexBufferObject()->containsUVs) {
		ImGui::InputText("Texture path: ", texturePath, sizeof(texturePath));
		sPath = texturePath;

		if (ImGui::Button("Add texture"))
		{
			DynamicTextured* newObject = new DynamicTextured(dynamicObject, _gameManager->LoadTexture(sPath), _gameManager->LoadSpecular(sPath));
			Node* oldLocation;
			_sceneGraph->FindObject(dynamicObject, oldLocation);
			_sceneGraph->ConvertObjectTo(newObject, oldLocation);
			currentObject = newObject;
		}
		ImGui::SameLine();
	}

	if (ImGui::Button("Delete Object"))
	{
		_sceneGraph->EmptySingle(dynamicObject);
	}

	if (ImGui::Button("Parent Object"))
	{
		parenting = true;
		printf("Parenting");
	}
	ImGui::SameLine();
	if (ImGui::Button("Unparent Object"))
	{
		Node* root = _sceneGraph->GetRoot();
		Node* object;
		_sceneGraph->FindObject(currentObject, object);
		_sceneGraph->MoveObjectTo(object, root);
	}
}
void UIManager::StaticTextureProperties(Object*& currentObject, bool &parenting)
{
	string sPath;
	glm::vec3 position;
	glm::vec3 rotationAngle;
	glm::vec3 scaleFactor;
	static char texturePath[32];
	static bool editName = false;

	StaticTextured* staticTexturedobject;
	staticTexturedobject = static_cast<StaticTextured*>(currentObject);
	position = staticTexturedobject->GetModelPosition();
	rotationAngle = staticTexturedobject->GetRotationAngle();
	scaleFactor = staticTexturedobject->GetScaleFactor();

	if (editName) {
		char* data;
		data = new char[128];
		sprintf(data, "%s", staticTexturedobject->GetName()->c_str());
		if (ImGui::InputText("Name", data, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
			staticTexturedobject->SetName(string(data));
			editName = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) editName = false;
	}
	else {
		ImGui::Text("Name: %s", staticTexturedobject->GetName()->c_str());
		ImGui::SameLine();
		if (ImGui::Button("Edit")) editName = true;
	}

	if (ImGui::DragFloat3("position", &position[0], 0.1f, -1000.0f, 1000.0f, "%.3fY")) staticTexturedobject->SetModelPosition(position);
	if (ImGui::DragFloat3("rotation", &rotationAngle[0], 0.1f, -180, 180)) staticTexturedobject->SetRotationAngle(rotationAngle[0], rotationAngle[1], rotationAngle[2]);
	if (ImGui::DragFloat3("scale", &scaleFactor[0], 0.01f, -5, 5))	staticTexturedobject->SetScaleFactor(scaleFactor);


	ImGui::DragFloat("Shininess: ", &staticTexturedobject->GetMaterial()->shininess, 0.01f, 1.0f, 256.0f);
	ImGui::InputText("Texture path: ", texturePath, sizeof(texturePath));
	sPath = texturePath;

	if (ImGui::Button("Change texture")) {
		staticTexturedobject->ChangeTexture(_gameManager->LoadTexture(sPath), _gameManager->LoadTexture(sPath));
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Object"))
	{
		_sceneGraph->EmptySingle(currentObject);
	}

	if (ImGui::Button("Parent Object"))
	{
		parenting = true;
		printf("Parenting");
	}
	ImGui::SameLine();
	if (ImGui::Button("Unparent Object"))
	{
		Node* root = _sceneGraph->GetRoot();
		Node* object;
		_sceneGraph->FindObject(currentObject, object);
		_sceneGraph->MoveObjectTo(object, root);
	}
}
void UIManager::StaticMaterialProperties(Object*& currentObject, bool &parenting)
{
	string sPath;
	glm::vec3 position;
	glm::vec3 rotationAngle;
	glm::vec3 scaleFactor;
	static char texturePath[32];
	static bool editName = false;

	StaticObject* staticObject;
	staticObject = static_cast<StaticObject*>(currentObject);
	position = staticObject->GetModelPosition();
	rotationAngle = staticObject->GetRotationAngle();
	scaleFactor = staticObject->GetScaleFactor();

	if (editName) {
		char* data;
		data = new char[128];
		sprintf(data, "%s", staticObject->GetName()->c_str());
		if (ImGui::InputText("Name", data, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
			staticObject->SetName(string(data));
			editName = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) editName = false;
	}
	else {
		ImGui::Text("Name: %s", staticObject->GetName()->c_str());
		ImGui::SameLine();
		if (ImGui::Button("Edit")) editName = true;
	}


	if (ImGui::DragFloat3("position", &position[0], 0.1f, -1000.0f, 1000.0f, "%.3f")) staticObject->SetModelPosition(position);
	if (ImGui::DragFloat3("rotation", &rotationAngle[0], 0.1f, -180, 180)) staticObject->SetRotationAngle(rotationAngle[0], rotationAngle[1], rotationAngle[2]);
	if (ImGui::DragFloat3("scale", &scaleFactor[0], 0.01f, 0.001f, 100.0f)) staticObject->SetScaleFactor(scaleFactor);

	ImGui::DragFloat3("ambient", &staticObject->GetMaterial()->ambient[0], 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat3("diffuse", &staticObject->GetMaterial()->diffuse[0], 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat3("specular", &staticObject->GetMaterial()->specular[0], 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat("Shininess", &staticObject->GetMaterial()->shininess, 0.01f, 1.0f, 256.0f);



	if (staticObject->GetVertexBufferObject()->containsUVs) {
		ImGui::InputText("Texture path: ", texturePath, sizeof(texturePath));
		sPath = texturePath;

		if (ImGui::Button("Add texture"))
		{
			StaticTextured* newObject = new StaticTextured(staticObject, _gameManager->LoadTexture(sPath), _gameManager->LoadSpecular(sPath));
			Node* oldLocation;
			_sceneGraph->FindObject(staticObject, oldLocation);
			_sceneGraph->ConvertObjectTo(newObject, oldLocation);
			currentObject = newObject;
		}
		ImGui::SameLine();
	}

	if (ImGui::Button("Delete Object"))
	{
		_sceneGraph->EmptySingle(staticObject);
	}

	if (ImGui::Button("Parent Object"))
	{
	parenting = true;
	printf("Parenting");
	}
	ImGui::SameLine();
	if (ImGui::Button("Unparent Object"))
	{
	Node* root = _sceneGraph->GetRoot();
	Node* object;
	_sceneGraph->FindObject(currentObject, object);
	_sceneGraph->MoveObjectTo(object, root);
	}
}
void UIManager::PointLightProperties(Object*& currentObject, bool &parenting)
{
	glm::vec3 position;
	static bool editName = false;

	PointLight* pointLight;
	pointLight = static_cast<PointLight*>(currentObject);
	position = pointLight->GetModelPosition();

	PntLight* pntLight;
	pntLight = pointLight->GetLightParameters();

	if (editName) {
		char* data;
		data = new char[128];
		sprintf(data, "%s", pointLight->GetName()->c_str());
		if (ImGui::InputText("Name", data, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
			pointLight->SetName(string(data));
			editName = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) editName = false;
	}
	else {
		ImGui::Text("Name: %s", pointLight->GetName()->c_str());
		ImGui::SameLine();
		if (ImGui::Button("Edit")) editName = true;
	}

	if (ImGui::DragFloat3("position", &position[0], 0.1f, 0.0f, 0.0f, "%.3f")) pointLight->SetModelPosition(position);

	ImGui::DragFloat3("ambient", &pntLight->ambient[0], 0.001f, 0.0f, 1.0f, "%.3f");
	ImGui::DragFloat3("diffuse", &pntLight->diffuse[0], 0.001f, 0.0f, 1.0f, "%.3f");
	ImGui::DragFloat3("specular", &pntLight->specular[0], 0.001f, 0.0f, 1.0f, "%.3f");
	ImGui::DragFloat("linear", &pntLight->linear, 0.001f, 0.00001f, 1.0f, "%.5f");
	ImGui::DragFloat("Quardratic", &pntLight->quadratic, 0.0001f, 0.000001f, 2.0f, "%.6f");

	if (ImGui::Button("Parent Object"))
	{
		parenting = true;
		printf("Parenting");
	}
	ImGui::SameLine();
	if (ImGui::Button("Unparent Object"))
	{
		Node* root = _sceneGraph->GetRoot();
		Node* object;
		_sceneGraph->FindObject(currentObject, object);
		_sceneGraph->MoveObjectTo(object, root);
	}
}
void UIManager::DirectionalLightProperties(Object*& currentObject)
{
	glm::vec3 position;
	static bool editName = false;

	DirectionalLight* directionalLight;
	directionalLight = static_cast<DirectionalLight*>(currentObject);
	position = directionalLight->GetModelPosition();

	DirLight* dirLight;
	dirLight = directionalLight->GetLightParameters();

	LightManager::directionalLight->GetLightParameters();

	ImGui::Text("Directional light");
	if (ImGui::DragFloat3("position", &position[0], 0.1f, 0.0f, 0.0f, "%.3f")) directionalLight->SetModelPosition(position);

	ImGui::DragFloat3("ambient", &dirLight->ambient[0], 0.001f, 0.0f, 1.0f, "%.3f");
	ImGui::DragFloat3("diffuse", &dirLight->diffuse[0], 0.001f, 0.0f, 1.0f, "%.3f");
	ImGui::DragFloat3("specular", &dirLight->specular[0], 0.001f, 0.0f, 1.0f, "%.3f");
}

void UIManager::UpdateObjectPropertiesWindow(Object*& currentObject, bool &parenting) {
	string sPath;
	if (ImGui::Begin("Object window", &_objectPropertiesOpen, _objectProperpertoesFlags) && currentObject != nullptr) {

		glm::vec3 position;
		glm::vec3 rotationAngle;
		glm::vec3 scaleFactor;
		glm::vec3 rotationVelocity;
		static char texturePath[32];
		static bool editName = false;

		switch (currentObject->GetType()) {
		case ObjectType::DYNAMIC_TEXTURED_OBJECT:
			DynamicTextureProperties(currentObject, parenting);
			break;
		case ObjectType::DYNAMIC_MATERIAL_OBJECT:
			DynamicMaterialProperties(currentObject, parenting);
			break;
		case ObjectType::STATIC_TEXTURED_OBJECT:
			StaticTextureProperties(currentObject, parenting);
			break;
		case ObjectType::STATIC_MATERIAL_OBJECT:
			StaticMaterialProperties(currentObject, parenting);
			break;
		case ObjectType::POINT_LIGHT:
			PointLightProperties(currentObject, parenting);
			break;
		case ObjectType::DIRECTIONAL_LIGHT:
			DirectionalLightProperties(currentObject);
			break;
		default:
			break;
		}
	}
	ImGui::End();
	return;
}

void UIManager::UpdateSceneGraphWindow(Object*& currentObject, bool &parenting)
{
	Object* sceneGraphSelection = nullptr;
	if (ImGui::Begin("SceneGraph", &_sceneGraphOpen, _sceneGraphFlags)) {
		sceneGraphSelection = _sceneGraph->ImGuiSceneGraph(currentObject);
	}
	ImGui::End();
	if (sceneGraphSelection != nullptr)
	{
		if (parenting && currentObject != sceneGraphSelection) {
			printf("Parented to: %s\n", sceneGraphSelection->GetName()->c_str());
			Node* parent;
			_sceneGraph->FindObject(sceneGraphSelection, parent);
			Node* selectedObjectNode;
			_sceneGraph->FindObject(currentObject, selectedObjectNode);
			_sceneGraph->MoveObjectTo(selectedObjectNode, parent);
			parenting = false;
		}
		else {
			currentObject = sceneGraphSelection;
		}
	}
}

void UIManager::UpdateObjectCreateWindow() {
	ImGui::Begin("Create New Object", &_createObjectOpen, _createObjectFlags);
	if (ImGui::CollapsingHeader("Static")) {
		static char modelPath[32];
		ImGui::InputText("Model Path: ", modelPath, sizeof(modelPath));
		string sPath = modelPath;
		if (ImGui::Button("Add Static Material"))
		{
			VBO* mesh = _gameManager->LoadMesh(sPath);
			StaticObject* newObject = new StaticObject(mesh, mesh->material, glm::vec3(0.0f));
			if (sPath != "") newObject->SetName(sPath);
			_sceneGraph->AddRootSibling(newObject);
		}
	}
	if (ImGui::CollapsingHeader("Dynamic")) {
		static char modelPath[32];
		ImGui::InputText("Model Path: ", modelPath, sizeof(modelPath));
		string sPath = modelPath;
		if (ImGui::Button("Add Dynamic Material"))
		{
			VBO* mesh = _gameManager->LoadMesh(sPath);
			DynamicObject* newObject = new DynamicObject(mesh, mesh->material, glm::vec3(0.0f));
			if (sPath != "") newObject->SetName(sPath);
			_sceneGraph->AddRootSibling(newObject);
		}
	}
	ImGui::End();
}

void UIManager::UpdateWindowMenu() {
	static bool createObjectLock = false;
	static bool objectPropertiesLock = false;
	static bool sceneGraphLock = false;


	if (ImGui::BeginMenu("Windows"))
	{
		if (ImGui::BeginMenu("Create Object")) {
			if (ImGui::MenuItem("Enabled", NULL, _createObjectOpen)) _createObjectOpen = !_createObjectOpen;
			if (ImGui::MenuItem("Lock window", NULL, createObjectLock)) createObjectLock = !createObjectLock;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Object Properties")) {
			if (ImGui::MenuItem("Enabled", NULL, _objectPropertiesOpen)) _objectPropertiesOpen = !_objectPropertiesOpen;
			if (ImGui::MenuItem("Lock window", NULL, objectPropertiesLock)) objectPropertiesLock = !objectPropertiesLock;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Scene Graph")) {
			if (ImGui::MenuItem("Enabled", NULL, _sceneGraphOpen)) _sceneGraphOpen = !_sceneGraphOpen;
			if (ImGui::MenuItem("Lock window", NULL, sceneGraphLock)) sceneGraphLock = !sceneGraphLock;
			ImGui::EndMenu();
		}


		ImGui::EndMenu();
	}
	_createObjectFlags = createObjectLock ? ImGuiWindowFlags_NoMove : 0;
	_objectProperpertoesFlags = objectPropertiesLock ? ImGuiWindowFlags_NoMove : 0;
	_sceneGraphFlags = sceneGraphLock ? ImGuiWindowFlags_NoMove : 0;
}

void UIManager::UpdateFileMenu() {
	bool saveWorkPopup = false;
	bool popupWindow = false;
	bool overridePopup = false;

	if (ImGui::BeginMenu("File")) {

		if (ImGui::MenuItem("New"))
		{
			popupWindow = true;
			_popupType = PopupType::New;
		}

		if (ImGui::MenuItem("Save"))
		{
			_jsonIo->SaveJson();
		}

		if (ImGui::MenuItem("Save As"))
		{
			popupWindow = true;
			_popupType = PopupType::SaveAs;
		}

		if (ImGui::MenuItem("Open"))
		{
			popupWindow = true;
			_popupType = PopupType::Open;
		}

		if (ImGui::MenuItem("Test Map", "F5"))
		{
			_jsonIo->SaveJson();
			_gameManager->SwitchGameState(GameState::GamePlayState);
		}

		if (ImGui::MenuItem("Exit", "Alt-F4"))
		{
			saveWorkPopup = true;
			_popupType = PopupType::Exit;
		}
		ImGui::EndMenu();
	}


	static char buff[32];
	if (popupWindow) {
		switch (_popupType) {
		case PopupType::New:
			sprintf(buff, "New file###fileWindow");
			break;
		case PopupType::SaveAs:
			sprintf(buff, "Save file as###fileWindow");
			break;
		case PopupType::Open:
			sprintf(buff, "Open file###fileWindow");
			break;
		default:
			break;
		}

		ImGui::OpenPopup(buff);
	}

	static char pathBuffer[255];

	if (ImGui::BeginPopupModal(buff, NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		switch (_popupType) {
		case PopupType::New:
			ImGui::Text("Create new map file: ");
			ImGui::SameLine();
			ImGui::InputText("###newFilePath", pathBuffer, 255);
			if (ImGui::Button("Create")) {
				overridePopup = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			break;
		case PopupType::SaveAs:
			ImGui::Text("Save map as: ");
			ImGui::SameLine();
			ImGui::InputText("###saveAsFilePath", pathBuffer, 255);
			if (ImGui::Button("Save"))
			{
				overridePopup = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			break;
		case PopupType::Open:
			ImGui::Text("Load map file: ");
			ImGui::SameLine();
			ImGui::InputText("###openFilePath", pathBuffer, 255);
			if (ImGui::Button("Open")) {
				saveWorkPopup = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			break;
		default:
			break;
		}
		ImGui::EndPopup();
	}


	if (overridePopup) {
		if (_popupType == PopupType::SaveAs || _popupType == PopupType::New)
		{
			std::ifstream fileCheck("saves/maps/" + string(pathBuffer) + ".json");
			if (fileCheck.good())
			{
				ImGui::OpenPopup("Override old save file?");
			}
			else {
				if (_popupType == PopupType::SaveAs) _jsonIo->SaveJson(pathBuffer);
				else if (_popupType == PopupType::New) saveWorkPopup = true;
			}
		}
	}

	if (ImGui::BeginPopupModal("Override old save file?"))
	{
		ImGui::Text("You are about to override an existing save file. Continue?");
		if (ImGui::Button("Override old save"))
		{
			switch (_popupType) {
			case PopupType::New:
				saveWorkPopup = true;
				break;
			case PopupType::SaveAs:
				_jsonIo->SaveJson(pathBuffer);
				break;
			}
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (saveWorkPopup) {
		ImGui::OpenPopup("Save current work?");
	}

	if (ImGui::BeginPopupModal("Save current work?"))
	{
		ImGui::Text("Would you like to save before continuing?");
		if (ImGui::Button("Save")) {
			_jsonIo->SaveJson();
			switch (_popupType)
			{
			case PopupType::New:
				_sceneGraph->EmptyTree();
				_gameManager->LoadNewMap();
				FileManager::currentFilePath = string(pathBuffer);
				break;
			case PopupType::Open:
				printf("Opening file...%s", pathBuffer);
				_jsonIo->LoadJson(pathBuffer);
				break;
			case PopupType::Exit:
				_gameManager->CloseGame();
				break;
			default:
				break;
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Don't save")) {
			switch (_popupType)
			{
			case PopupType::New:
				_sceneGraph->EmptyTree();
				_gameManager->LoadNewMap();
				FileManager::currentFilePath = string(pathBuffer);
				break;
			case PopupType::Open:
				printf("Opening file...%s", pathBuffer);
				_jsonIo->LoadJson(pathBuffer);
				break;
			case PopupType::Exit:
				_gameManager->CloseGame();
				break;
			default:
				break;
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void UIManager::UpdateMenuBar() {
	if (ImGui::BeginMainMenuBar()) {

		UpdateFileMenu();

		UpdateWindowMenu();
	}
	ImGui::EndMainMenuBar();
}

void UIManager::UpdateUI(Object*& currentObject, bool &parenting) {
	if(_objectPropertiesOpen) UpdateObjectPropertiesWindow(currentObject, parenting);
	if(_createObjectOpen) UpdateObjectCreateWindow();
	if (_sceneGraphOpen)UpdateSceneGraphWindow(currentObject, parenting);
	else _sceneGraph->UpdateSceneGraphObject();
	UpdateMenuBar();
}