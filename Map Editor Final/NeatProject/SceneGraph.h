#pragma once

#include "RootObject.h"
#include "SceneObject.h"
#include "StaticObject.h"

struct Node
{
	Node(Object* data) {
		Node::data = data;
		child = nullptr;
		sibling = nullptr;
	}

	Object* data;
	Node* child, *sibling;
};

static Object* rootObject(new RootObject());

class SceneGraph
{
private:
	Node *_root;
	Node *_selectedNode;
	const Object *_searchItem;

	void DrawSceneGraphObjects(Node* node);
	void UpdateSceneGraphObjects(Node* node, glm::mat4 &parentMatrix);
	void GetAllObjects(Node* node, vector<Node*> &list);
public:

	SceneGraph();
	~SceneGraph();

	Node* FollowSiblings(Node* node);
	Node* SearchFrom(Node* node);
	//Start from given node
	Node* FindLinker(Node* node);
	//Start from root
	Node* FindLinker();
	bool FindObject(const Object* object, Node *& output);
	void GetAllSiblings(Node* node, vector<Node*> &list);
	void GetAllChildren(Node* node, vector<Node*> &list);
	void GetAllObjects(vector<Node*> &list);
	Node* GetRoot() { return _root; };
	
	Node* AddRootSibling(Object* object);
	Node* AddChild(Object* child, Node* parent);

	void MoveObjectTo(Node* oldLocation, Node* newParent);
	void ConvertObjectTo(Object* newObject, Node* oldObject);

	Object* ImGuiSceneBranch(Node* node, glm::mat4& parentMatrix);
	Object* ImGuiSceneGraph(Object* selectedObject);

	void DrawSceneGraphObjects();

	void UpdateSceneGraphObject();

	void EmptyFrom(Node* &node);
	void EmptySingle(Object* object);
	void EmptyTree();
};

