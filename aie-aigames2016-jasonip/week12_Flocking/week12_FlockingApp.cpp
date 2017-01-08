#include "week12_FlockingApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"

week12_FlockingApp::week12_FlockingApp() {

}

week12_FlockingApp::~week12_FlockingApp() {

}

bool week12_FlockingApp::startup() {
	
	m_2dRenderer = new aie::Renderer2D();

	m_font = new aie::Font("./font/consolas.ttf", 32);

	// setup player
	m_keyboardBehaviour.setSpeed(400);
	m_player.setPosition(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);
	m_player.addBehaviour(&m_keyboardBehaviour);

	m_seek.setTarget(&m_player);

	m_entities.resize(100);

	m_seperation.setEntities(&m_entities);
	m_seperation.setRadius(200.0f);

	m_cohesion.setEntities(&m_entities);
	m_cohesion.setRadius(200.0f);

	m_alignment.setEntities(&m_entities);
	m_alignment.setRadius(200.0f);

//	m_steeringBehaviour.addForce(&m_wander, 1.f);
	m_steeringBehaviour.addForce(&m_seperation, 1.5f);
	m_steeringBehaviour.addForce(&m_cohesion, 1.f);
//	m_steeringBehaviour.addForce(&m_alignment, 1.f);
	m_steeringBehaviour.addForce(&m_seek, 0.5f);

	for (auto& entity : m_entities) {
		
		float a = rand() / (float)RAND_MAX * 3.14159f * 2;
		Vector2* v = new Vector2();
		v->x = sinf(a) * 150.0f;
		v->y = cosf(a) * 150.0f;
		entity.getBlackboard().set("velocity", v, true);
		entity.getBlackboard().set("maxForce", 250.f);
		entity.getBlackboard().set("maxVelocity", 100.f);

		WanderData* wd = new WanderData();
		wd->offset = 100;
		wd->radius = 75;
		wd->jitter = 25;
		wd->x = 0;
		wd->y = 0;

		entity.getBlackboard().set("wanderData", wd, true);

		entity.addBehaviour(&m_steeringBehaviour);

		entity.setPosition(rand() / (float)RAND_MAX * getWindowWidth(),
			rand() / (float)RAND_MAX * getWindowHeight());
	}

	return true;
}

void week12_FlockingApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void week12_FlockingApp::update(float deltaTime) {

	m_player.update(deltaTime);
	for (auto& entity : m_entities) {
		entity.update(deltaTime);
	}

	aie::Input* input = aie::Input::getInstance();

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void week12_FlockingApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	float x, y;

	m_player.getPosition(&x, &y);
	screenWrap(x, y);
	m_player.setPosition(x, y);
	m_2dRenderer->setRenderColour(0, 1, 0);
	m_2dRenderer->drawBox(x, y, 4, 4, 0, 0);

	m_2dRenderer->setRenderColour(1, 1, 1);
	for (auto& entity : m_entities) {
		entity.getPosition(&x, &y);
		screenWrap(x, y);
		entity.setPosition(x, y);

		m_2dRenderer->drawBox(x, y, 4, 4, 0, 0);
	}

	// output some text
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}

void week12_FlockingApp::screenWrap(float& x, float& y) {
	x = fmod(x, (float)getWindowWidth());
	if (x < 0)
		x += getWindowWidth();

	y = fmod(y, (float)getWindowHeight());
	if (y < 0)
		y += getWindowHeight();
}