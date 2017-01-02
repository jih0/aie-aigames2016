#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "GameObject.h"
#include "KeyboardBehaviour.h"
#include "SteeringBehaviour.h"

class week3_SteeringBehavioursApp : public aie::Application {
public:

	week3_SteeringBehavioursApp();
	virtual ~week3_SteeringBehavioursApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;

	GameObject			m_player;
	KeyboardBehaviour	m_keyboardBehaviour;

	GameObject			m_enemies[50];
	SteeringBehaviour	m_steeringBehaviour;
	SeekForce			m_seek;
	FleeForce			m_flee;
	WanderForce			m_wander;
};