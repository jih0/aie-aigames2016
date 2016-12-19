#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "GameObject.h"
#include "KeyboardBehaviour.h"
#include "FollowBehaviour.h"

class week1_AgentsApp : public aie::Application {
public:

	week1_AgentsApp();
	virtual ~week1_AgentsApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;

	GameObject			m_player;
	KeyboardBehaviour	m_keyboardBehaviour;

	GameObject			m_enemy;
	FollowBehaviour		m_followBehaviour;
};