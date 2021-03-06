#pragma once
#define _USE_MATH_DEFINES

#include "Application.h"
#include "Renderer2D.h"
#include "Audio.h"
#include "Texture.h"

#include "GameObject.h"
#include "Behaviour.h"
#include "SteeringBehaviour.h"
#include "KeyboardBehaviour.h"
#include "Pathfinding.h"

#include <math.h>

class AssessmentApp : public aie::Application {
public:

	AssessmentApp();
	virtual ~AssessmentApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	void screenWrap(float &x, float &y);
	void calcSpriteRotation(GameObject* gameObject);

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;

	aie::Texture		m_map;
	aie::Texture		m_spriteSheet;
	aie::Texture		m_charSpriteSheet;
	aie::Texture		m_soldierSprite;
	aie::Texture		m_zombieSprite;

	float				m_timer;
	float				m_detectRadius;
	int					m_enemyNum;
	int					m_soldierNum;

	GameObject			m_player;
	KeyboardBehaviour	m_keyboardBehaviour;

	// soldiers
	std::vector<GameObject>		m_soldiers;
	std::vector<FiniteStateMachine>	m_soldierFSM;
	PathBehaviour m_pathBehaviour;

	std::vector<MyNode*>						m_sSpawn;
	std::vector<MyNode*>						m_nodes;
	std::list<Pathfinding::Node*>				m_path;
	std::vector<std::list<Pathfinding::Node*>>	m_pathVector;

	SeekForce				m_sSeek;
	WanderForce				m_sWander;
	ObstacleAvoidanceForce	m_sAvoid;

	MyNode* m_start;
	MyNode* m_end;
	
	// zombies
	std::vector<GameObject>			m_enemies;
	std::vector<FiniteStateMachine>	m_enemyFSM;

	std::vector<Vector2>			m_eSpawn;
//	GameObject			m_enemies[10];
//	FiniteStateMachine	m_fsm[50];
//	FiniteStateMachine	m_zombieFSM;

//	SteeringBehaviour		m_steeringBehaviour;
	WanderForce				m_eWander;
	SeekForce				m_eSeek;
	IdleForce				m_eIdle;
	CohesionForce			m_eCohesion;
	SeperationForce			m_eSeperation;
	AlignmentForce			m_eAlignment;
	ObstacleAvoidanceForce	m_eAvoid;
	
	struct Box {
		float x, y, w, h;
	};

	struct Circle {
		float x, y, r;
	};

//	std::vector<Obstacle>	m_obstacles;
	std::vector<Circle>		m_circleObstacles;
	std::vector<Box>		m_boxObstacles;
};