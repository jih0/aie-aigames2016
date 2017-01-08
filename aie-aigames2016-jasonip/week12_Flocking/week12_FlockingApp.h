#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "Audio.h"

#include "GameObject.h"
#include "SteeringBehaviour.h"
#include "KeyboardBehaviour.h"

class week12_FlockingApp : public aie::Application {
public:

	week12_FlockingApp();
	virtual ~week12_FlockingApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	void screenWrap(float& x, float& y);

	aie::Renderer2D*	m_2dRenderer;
	aie::Texture*		m_texture;
	aie::Texture*		m_shipTexture;
	aie::Font*			m_font;
	aie::Audio*			m_audio;

	float m_cameraX, m_cameraY;
	float m_timer;

	std::vector<GameObject>	m_entities;

	SteeringBehaviour	m_steeringBehaviour;

	WanderForce			m_wander;
	SeperationForce		m_seperation;
	CohesionForce		m_cohesion;
	AlignmentForce		m_alignment;

	GameObject			m_player;
	KeyboardBehaviour	m_keyboardBehaviour;
	SeekForce			m_seek;
};