#include "SceneGraph.h"

#include "ImGui\imgui.h"

SceneGraph::SceneGraph()
{
	rootObject->SetName("Root");
	_root = new Node(rootObject);
}


SceneGraph::~SceneGraph()
{
}

Node* SceneGraph::FollowSiblings(Node* node)
{
	if (node->sibling != nullptr)
	{
		return FollowSiblings(node->sibling);
	}
	return node;
}

Node* SceneGraph::FindLinker()
{
	return FindLinker(_root);
}

Node* SceneGraph::FindLinker(Node* node)
{
	Node* tempReturn = nullptr;

	if (node->child != nullptr)
	{
		if(node->child->data != _searchItem) tempReturn = FindLinker(node->child);
		else tempReturn = node;
	}

	if (node->sibling != nullptr && tempReturn == nullptr)
	{
		if(node->sibling->data != _searchItem) tempReturn = FindLinker(node->sibling);
		else tempReturn = node;
	}

	return tempReturn;
}

Node* SceneGraph::SearchFrom(Node* node)
{
	Node* tempReturn = nullptr;

	if (node->data != _searchItem) {
		if (node->child != nullptr)
		{
			tempReturn = SearchFrom(node->child);
		}

		if (node->sibling != nullptr && tempReturn == nullptr)
		{
			tempReturn = SearchFrom(node->sibling);
		}
		return tempReturn;
	}
	else {
		return node;
	}
}

bool SceneGraph::FindObject(const Object* object, Node *& output)
{
	_searchItem = object;
	output = SearchFrom(_root);
	if (output != nullptr)
	{
		return true;
	}
	return false;
}

void SceneGraph::GetAllSiblings(Node* node, vector<Node*> &list)
{
	list.push_back(node);
	if (node->sibling != nullptr)
	{
		GetAllSiblings(node->sibling, list);
	}
	
	return;
}

void SceneGraph::GetAllChildren(Node* node, vector<Node*> &list)
{
	if (node->child != nullptr) {
		GetAllSiblings(node->child, list);
	}

	return;
}

void SceneGraph::GetAllObjects(Node* node, vector<Node*> &list)
{
	if (node->child != nullptr)
	{
		GetAllObjects(node->child, list);
	}

	list.push_back(node);

	if (node->sibling != nullptr)
	{
		GetAllObjects(node->sibling, list);
	}

	return;
}

void SceneGraph::GetAllObjects(vector<Node*> &list)
{
	GetAllObjects(_root, list);
}

Node* SceneGraph::AddRootSibling(Object* object)
{
	if (_root->child != nullptr)
	{
		return (FollowSiblings(_root->child)->sibling = new Node(object));
	}
	else {
		return (_root->child = new Node(object));
	}
}

Node* SceneGraph::AddChild(Object* child, Node* parent)
{
	if (parent != nullptr) {
		if (parent->child != nullptr)
		{
			return (FollowSiblings(parent->child)->sibling = new Node(child));
		}
		else {
			return (parent->child = new Node(child));
		}
	}
	return nullptr;
}

void SceneGraph::MoveObjectTo(Node* oldLocation, Node* newParent)
{
	_searchItem = oldLocation->data;
	Node* linker = FindLinker();

	
	_searchItem = newParent->data;
	Node* heirachyCheck = nullptr;
	if(oldLocation->child != nullptr) heirachyCheck = SearchFrom(oldLocation->child);

	if (heirachyCheck != nullptr)
	{
		printf("Cannot move parent to child\n");
		return;
	}

	if (linker->sibling == oldLocation)
	{
		linker->sibling = oldLocation->sibling;
	}
	else if (linker->child == oldLocation)
	{
		if (oldLocation->sibling == nullptr) {
			linker->child = nullptr;
		}
		else {
			linker->child = oldLocation->sibling;
		}
	}
	oldLocation->sibling = nullptr;

	if (newParent->child != nullptr)
	{
		FollowSiblings(newParent->child)->sibling = oldLocation;
	}
	else {
		newParent->child = oldLocation;
	}
	
	return;
}

void SceneGraph::ConvertObjectTo(Object* newObject, Node* oldObject)
{
	//delete oldObject->data;
	oldObject->data = newObject;
}

Object* SceneGraph::ImGuiSceneBranch(Node* node, glm::mat4& parentMatrix)
{
	node->data->Update(parentMatrix);

	static ImGuiTreeNodeFlags treeFlags;

	if (_selectedNode == node)
	{
		treeFlags = ImGuiTreeNodeFlags_Selected;
	}
	else {
		treeFlags = 0;
	}

	Object* returnValue = nullptr;

	if (node->child != nullptr)
	{
		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)node, treeFlags, node->data->GetName()->c_str());
		if (ImGui::IsItemClicked()) {
			_selectedNode = node;
			returnValue = node->data;
		}
		if (nodeOpen) {
			returnValue = ImGuiSceneBranch(node->child, node->data->GetModelMatrix());
			ImGui::TreePop();
		}
		else {
			UpdateSceneGraphObjects(node->child, node->data->GetModelMatrix());
		}
	}
	else {
		ImGui::TreeNodeEx((void*)(intptr_t) node, treeFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, node->data->GetName()->c_str());
		if (ImGui::IsItemClicked()) {
			_selectedNode = node;
			returnValue = node->data;
		}
	}
	if (node->sibling != nullptr && returnValue == nullptr)
	{
		returnValue = ImGuiSceneBranch(node->sibling, parentMatrix);
	}

	return returnValue;
}

Object* SceneGraph::ImGuiSceneGraph(Object* selectedObject)
{
	if (_selectedNode != nullptr && selectedObject != nullptr) {
		if (selectedObject != _selectedNode->data)
		{
			Node* newNode;
			FindObject(selectedObject, newNode);
			_selectedNode = newNode;
		}
	}
	else if (_selectedNode == nullptr)
	{
		Node* newNode;
		FindObject(selectedObject, newNode);
		_selectedNode = newNode;
	}

	Object* selectedNodeObject = ImGuiSceneBranch(_root->child, glm::mat4());

	return selectedNodeObject;
}

void SceneGraph::DrawSceneGraphObjects(Node* node)
{
	node->data->Draw();
	

	if (node->child != nullptr)
	{
		DrawSceneGraphObjects(node->child);
	}
	if (node->sibling != nullptr)
	{
		DrawSceneGraphObjects(node->sibling);
	}
	return;
}

void SceneGraph::DrawSceneGraphObjects()
{
	DrawSceneGraphObjects(_root);
}

void SceneGraph::UpdateSceneGraphObjects(Node* node, glm::mat4 &parentMatrix)
{
	node->data->Update(parentMatrix);

	if (node->child != nullptr)
	{
		UpdateSceneGraphObjects(node->child, node->data->GetModelMatrix());
	}

	if (node->sibling != nullptr)
	{
		UpdateSceneGraphObjects(node->sibling, parentMatrix);
	}
}

void SceneGraph::UpdateSceneGraphObject()
{
	UpdateSceneGraphObjects(_root, glm::mat4());
}

void SceneGraph::EmptyFrom(Node* &node)
{
	if (node->child != nullptr)
	{
		EmptyFrom(node->child);
	}

	if (node->sibling != nullptr)
	{
		EmptyFrom(node->sibling);
	}

	delete node->data;
	delete node;
	node = nullptr;
	return;
}

void SceneGraph::EmptySingle(Object* object)
{
	_searchItem = object;
	Node* linkerNode = FindLinker();
	Node* objectNode;
	if (linkerNode->child != nullptr && linkerNode->child->data == object)
	{
		objectNode = linkerNode->child;

		if (linkerNode->child->child) {
			FollowSiblings(objectNode->child)->sibling = objectNode->sibling;
			linkerNode->child = objectNode->child;
		}
		else {
			linkerNode->child = objectNode->sibling;
		}
		delete objectNode->data;
		delete objectNode;
	}
	else if (linkerNode->sibling != nullptr && linkerNode->sibling->data == object)
	{
		objectNode = linkerNode->sibling;

		if (linkerNode->sibling->child) {
			FollowSiblings(objectNode->child)->sibling = objectNode->sibling;
			linkerNode->sibling = objectNode->child;
		}
		else {
			linkerNode->sibling = objectNode->sibling;
		}
		delete objectNode->data;
		delete objectNode;
	}
	return;
}

void SceneGraph::EmptyTree()
{
	if(_root->child != nullptr) EmptyFrom(_root->child);
}