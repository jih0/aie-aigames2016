#pragma once
#define _USE_MATH_DEFINES

#include "Application.h"
#include "Renderer2D.h"
#include "Audio.h"
#include "Texture.h"

#include "GameObject.h"
#include "SteeringBehaviour.h"
#include "KeyboardBehaviour.h"

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

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;

	aie::Texture		m_map;
	aie::Texture		m_spriteSheet;
	aie::Texture		m_charSpriteSheet;

	float				m_timer;
	float				m_detectRadius;

	GameObject			m_player;
	KeyboardBehaviour	m_keyboardBehaviour;

	std::vector<GameObject>	m_enemies;
	std::vector<FiniteStateMachine>	m_enemyFSM;
//	GameObject			m_enemies[10];
//	FiniteStateMachine	m_fsm[50];
//	FiniteStateMachine	m_zombieFSM;

	SteeringBehaviour	m_steeringBehaviour;
	WanderForce			m_wander;
	SeekForce			m_seek;
	IdleForce			m_idle;
	CohesionForce		m_cohesion;
	SeperationForce		m_seperation;
	AlignmentForce		m_alignment;
};