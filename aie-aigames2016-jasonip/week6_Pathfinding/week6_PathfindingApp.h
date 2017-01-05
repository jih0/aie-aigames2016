#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "Pathfinding.h"
#include "GameObject.h"
#include "Behaviour.h"

class MyNode : public Pathfinding::Node {
public:

	MyNode() {}
	virtual ~MyNode() {}

	float x, y;
};

class PathBehaviour : public Behaviour {
public:

	PathBehaviour() {}
	virtual ~PathBehaviour() {}

	virtual bool execute(GameObject* gameObject, float deltaTime);

	std::vector<MyNode*>* nodes;
};

class week6_PathfindingApp : public aie::Application {
public:

	week6_PathfindingApp();
	virtual ~week6_PathfindingApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;

	std::vector<MyNode*> m_nodes;
	std::list<Pathfinding::Node*> m_path;

	MyNode* m_start;
	MyNode* m_end;

	GameObject m_player;
	PathBehaviour m_pathBehaviour;
};