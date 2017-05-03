#pragma once

#include "ImGui\imgui.h"

#include "IOFileManager.h"
#include "SceneGraph.h"

#include "Object.h"

class GameManager;

enum PopupType { New, SaveAs, Open, Exit };

class UIManager
{
private:
	GameManager* _gameManager;
	SceneGraph* _sceneGraph;
	JSONIOFileManager* _jsonIo;

	PopupType _popupType;

	bool _objectPropertiesOpen, _sceneGraphOpen, _createObjectOpen;
	ImGuiWindowFlags _objectProperpertoesFlags, _sceneGraphFlags, _createObjectFlags;
public:
	UIManager(GameManager* gameManager, SceneGraph* sceneGraph);
	~UIManager();

	void SaveJson() { _jsonIo->SaveJson(); };

	void DynamicTextureProperties(Object*& currentObject, bool &parenting);
	void DynamicMaterialProperties(Object*& currentObject, bool &parenting);
	void StaticTextureProperties(Object*& currentObject, bool &parenting);
	void StaticMaterialProperties(Object*& currentObject, bool &parenting);
	void PointLightProperties(Object*& currentObject, bool &parenting);
	void DirectionalLightProperties(Object*& currentObject);
	void UpdateObjectPropertiesWindow(Object*& currentObject, bool &parenting);

	void UpdateSceneGraphWindow(Object*& currentObject, bool &parenting);
	void UpdateObjectCreateWindow();
	void UpdateWindowMenu();
	void UpdateFileMenu();
	void UpdateMenuBar();

	void UpdateUI(Object *& currentObject, bool &parenting);
};

