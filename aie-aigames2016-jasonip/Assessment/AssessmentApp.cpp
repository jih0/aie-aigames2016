#include "AssessmentApp.h"
#include "Font.h"
#include "Input.h"

#include <ctime>
#include <string>

AssessmentApp::AssessmentApp() {

}

AssessmentApp::~AssessmentApp() {

}

bool AssessmentApp::startup() {
	
	m_detectRadius = 350;
	m_enemyNum = 1;
	m_soldierNum = 1;

	m_soldiers.resize(m_soldierNum);
	m_soldierFSM.resize(m_soldierNum);
	m_pathVector.resize(m_soldierNum);

	m_enemies.resize(m_enemyNum);
	m_enemyFSM.resize(m_enemyNum);

	m_sSeek.setTarget(&m_enemies);
	m_eSeek.setTarget(&m_soldiers);

	m_spriteSheet.load("./textures/spritesheet_tiles.png");
	m_charSpriteSheet.load("./textures/spritesheet_characters.png");
	
	// obtain time information to use as rand() seed information
	// used to generate pseudo-procedurally generated assets
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	unsigned int seed = (unsigned int)timeinfo->tm_sec;
	srand(seed);

	// set up background
	//m_background.load("./map/map0.png");

	m_2dRenderer = new aie::Renderer2D();

	m_font = new aie::Font("./font/consolas.ttf", 32);

	// set up player
	m_keyboardBehaviour.setSpeed(400);
	m_player.setPosition(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);
	m_player.addBehaviour(&m_keyboardBehaviour);

	m_map.load("./map/map2.png");

	auto pixels = m_map.getPixels();
	auto channels = m_map.getFormat();

	// create nodes, using map1.png
	for (unsigned int x = 0; x < m_map.getWidth(); ++x) {
		for (unsigned int y = 0; y < m_map.getHeight(); ++y) {

			int index = (y * m_map.getWidth() + x) * channels;

			if (pixels[index + 0] == 255 &&
				pixels[index + 1] == 255 &&
				pixels[index + 2] == 255) continue; // ignore it if the pixel is white; this will be filled by the background

			if (pixels[index + 0] == 0 &&
				pixels[index + 1] == 0 &&
				pixels[index + 2] == 255) { // blue is water, and is to be avoided by all

				Box b;
				b.x = (20 * x) + 10;
				b.y = getWindowHeight() - ((20 * y) + 10);
				b.w = 20;
				b.h = 20;

				m_boxObstacles.push_back(b);
				m_eAvoid.addObstacle(b.x, b.y, 0, b.w, b.h);
				m_sAvoid.addObstacle(b.x, b.y, 0, b.w, b.h);
			}

			if (pixels[index + 0] == 255 &&
				pixels[index + 1] == 0 &&
				pixels[index + 2] == 0) { // red is our pathfinding mesh

				MyNode* node = new MyNode();
				node->x = float(x * 20 + 10); // 20 pixel seperation starting 100 pixels in
				node->y = getWindowHeight() - float(y * 20 + 10);

				m_nodes.push_back(node);
			}

			if (pixels[index + 0] == 0 &&
				pixels[index + 1] == 255 &&
				pixels[index + 2] == 255) { // teal is our soldier respawn nodes

				MyNode* node = new MyNode();
				node->x = float(x * 20 + 10); // 20 pixel seperation starting 100 pixels in
				node->y = getWindowHeight() - float(y * 20 + 10);

				m_sSpawn.push_back(node);
				m_nodes.push_back(node);
			}

			if (pixels[index + 0] == 0 &&
				pixels[index + 1] == 0 &&
				pixels[index + 2] == 0) { // black is our zombie spawn point

				Vector2 v;
				v.x = (20 * x) + 10;
				v.y = getWindowHeight() - ((20 * y) + 10);

				m_eSpawn.push_back(v);
			}
		}
	}

	// create edges/links
	for (auto a : m_nodes) {
		for (auto b : m_nodes) {
			if (a == b) continue;

			float x = b->x - a->x;
			float y = b->y - a->y;
			float sqrDist = x * x + y * y;

			if (sqrDist <= (30 * 30)) {
				Pathfinding::Edge edge;
				edge.cost = sqrDist;
				edge.target = b;

				a->edges.push_back(edge);
			}
		}
	}

	// set up soldierFSM

	m_sAvoid.setFeelerLength(20);

	// created new states
	PathState* pathfindState = new PathState();

	SteeringState* sAttackState = new SteeringState();
	sAttackState->addForce(&m_sSeek, 1.0f);
	sAttackState->addForce(&m_sAvoid, 0.5f);

	SteeringState* sDeadState = new SteeringState();

	SteeringState* sWanderState = new SteeringState();
	sWanderState->addForce(&m_sWander, 1.0f);
	sWanderState->addForce(&m_sAvoid, 0.5f);

	// setup conditions that will trigger transition
	Condition* sWithinRangeCondition = new WithinRangeCondition(&m_enemies, m_detectRadius);
	Condition* sNotWithinRangeCondition = new NotCondition(sWithinRangeCondition);
	Condition* sIsDeadCondition = new DeadCondition();
	Condition* sIsNotDeadCondition = new NotCondition(sIsDeadCondition);
	Condition* sTargetKilledCondition = new TargetKilledCondition(&m_enemies);
	Condition* sWanderTimeOutCondition = new FloatGreaterCondition(sWanderState->getTimerPtr(), 2);

	// add transitions
	Transition* sWithinRange = new Transition(sAttackState, sWithinRangeCondition);
	Transition* sNotWithinRange = new Transition(pathfindState, sNotWithinRangeCondition);
	Transition* sIsNotDead = new Transition(pathfindState, sIsNotDeadCondition);
	Transition* sIsDead = new Transition(sDeadState, sIsDeadCondition);
	Transition* sTargetKilled = new Transition(sWanderState, sTargetKilledCondition);
	Transition* sWanderTimeOut = new Transition(pathfindState, sWanderTimeOutCondition);

	// add transitions to states
	sAttackState->addTransition(sNotWithinRange);
	sAttackState->addTransition(sIsDead);
	sAttackState->addTransition(sTargetKilled);
	pathfindState->addTransition(sWithinRange);
	sDeadState->addTransition(sIsNotDead);
	sWanderState->addTransition(sWanderTimeOut);

	int i = 0;
	// set up soldiers
	for (auto& soldier : m_soldiers) {
		
		soldier.getBlackboard().set("path", &m_pathVector[i]);
		soldier.getBlackboard().set("speed", 30.0f);

		float stateTimer = 0.0f;

		Vector2* v = new Vector2();
		v->x = 0;
		v->y = 0;

		soldier.getBlackboard().set("isAlive", true);
		soldier.getBlackboard().set("HP", 5);
		soldier.getBlackboard().set("AP", 6);
		soldier.getBlackboard().set("attackRange", 15.0f);
		soldier.getBlackboard().set("attacking", false);
		soldier.getBlackboard().set("targetIndex", 0);
		soldier.getBlackboard().set("attackCooldown", 1.0f); // soldiers attack once per second
		soldier.getBlackboard().set("stateTimer", 0.0f);
		soldier.getBlackboard().set("velocity", v, true);
		soldier.getBlackboard().set("maxForce", 150.f);
		soldier.getBlackboard().set("maxVelocity", 60.f);
		soldier.getBlackboard().set("spriteRotation", 0.0f);

		// defining initial start and end nodes
		unsigned int spawnPt = rand() % 3;
		m_start = m_sSpawn[spawnPt];
		m_end = m_nodes[rand() % m_nodes.size()];

		soldier.setPosition(m_start->x, m_start->y);

		m_soldierFSM[i].setInitialState(pathfindState);
		soldier.addBehaviour(&m_soldierFSM[i]);
		pathfindState->nodes = &m_nodes;

		// perform first path search
		Pathfinding::Search::dijkstra(m_start, m_end, m_pathVector[i]);

		++i;
	}

	// set up zombieFSM

	m_eAvoid.setFeelerLength(20);

	// created new states
	SteeringState* eWanderState = new SteeringState();
	eWanderState->addForce(&m_eWander, 1.0f);
	eWanderState->addForce(&m_eAvoid, 1.0f);

	SteeringState* eIdleState = new SteeringState();
	eIdleState->addForce(&m_eIdle, 1.0f);

	SteeringState* eAttackState = new SteeringState();
	eAttackState->addForce(&m_eSeek, 1.0f);
	eAttackState->addForce(&m_eAvoid, 1.0f);

	SteeringState* eDeathState = new SteeringState();
	eDeathState->addForce(&m_eIdle, 1.0f);
	
	// setup conditions that will trigger transition
	Condition* eWithinRangeCondition = new WithinRangeCondition(&m_soldiers, m_detectRadius);
	Condition* eNotWithinRangeCondition = new NotCondition(eWithinRangeCondition);
	Condition* eWanderTimeOutCondition = new FloatGreaterCondition(eWanderState->getTimerPtr(), 5);
	Condition* eIdleTimeOutCondition = new FloatGreaterCondition(eIdleState->getTimerPtr(), 0);
	Condition* eIsDeadCondition = new DeadCondition();
	Condition* eIsNotDeadCondition = new NotCondition(eIsDeadCondition);
	Condition* eTargetKilledCondition = new TargetKilledCondition(&m_soldiers);

	// add transitions
	Transition* eIdleWithinRange = new Transition(eAttackState, eWithinRangeCondition);
	Transition* eWanderWithinRange = new Transition(eAttackState, eWithinRangeCondition);
	Transition* eNotWithinRange = new Transition(eWanderState, eNotWithinRangeCondition);
	Transition* eWanderTimeOut = new Transition(eIdleState, eWanderTimeOutCondition);
	Transition* eIdleTimeOut = new Transition(eWanderState, eIdleTimeOutCondition);
	Transition* eIsNotDead = new Transition(eWanderState, eIsNotDeadCondition);
	Transition* eIsDead = new Transition(eDeathState, eIsDeadCondition);
	Transition* eTargetKilled = new Transition(eWanderState, eTargetKilledCondition);

	// add transitions to states
	eAttackState->addTransition(eNotWithinRange);
	eAttackState->addTransition(eIsDead);
	eAttackState->addTransition(eTargetKilled);
	eWanderState->addTransition(eWanderWithinRange);
	eWanderState->addTransition(eWanderTimeOut);
	eIdleState->addTransition(eIdleWithinRange);
	eIdleState->addTransition(eIdleTimeOut);
	eDeathState->addTransition(eIsNotDead);
		
	i = 0;
	// set up enemies
	for (auto& enemy : m_enemies) {

		m_enemyFSM[i].setInitialState(eIdleState);
		enemy.addBehaviour(&m_enemyFSM[i]);
		++i;

		float stateTimer = 0.0f;

		Vector2* v = new Vector2();
		v->x = 0;
		v->y = 0;

		WanderData* wd = new WanderData();
		wd->offset = 100;
		wd->radius = 75;
		wd->jitter = 25;
		wd->x = 0;
		wd->y = 0;
		
		enemy.getBlackboard().set("isAlive", true);
		enemy.getBlackboard().set("HP", 3);
		enemy.getBlackboard().set("AP", 4);
		enemy.getBlackboard().set("attackRange", 15.0f);
		enemy.getBlackboard().set("attacking", false);
		enemy.getBlackboard().set("targetIndex", 0);
		enemy.getBlackboard().set("attackCooldown", 0.5f); // zombies attack once per 1.5 seconds
		enemy.getBlackboard().set("stateTimer", 0.0f);
		enemy.getBlackboard().set("velocity", v, true);
		enemy.getBlackboard().set("maxForce", 150.f);
		enemy.getBlackboard().set("maxVelocity", 50.f);
		enemy.getBlackboard().set("wanderData", wd, true);
		enemy.getBlackboard().set("spriteRotation", 0.0f);

		unsigned int spawnPt = rand() % 10; 
		enemy.setPosition( m_eSpawn[spawnPt].x, m_eSpawn[spawnPt].y );

	}

	return true;
}

void AssessmentApp::shutdown() {

	for (auto node : m_nodes)
		delete node;

	delete m_font;
	delete m_2dRenderer;
}

void AssessmentApp::update(float deltaTime) {

	m_timer += deltaTime;
	
	// update player
	m_player.update(deltaTime);

	// update soldiers
	for (auto& soldier : m_soldiers) {
		bool alive;
		soldier.getBlackboard().get("isAlive", alive);
		if (alive) {
			int index;
			soldier.getBlackboard().get("targetIndex", index);
			soldier.update(&m_enemies.at(index), deltaTime);
		}
		//else {
		//	int deathTimer;
		//	soldier.getBlackboard().get("stateTimer", deathTimer);
		//	if (deathTimer > 5.0f) {
		//		unsigned int spawnPt = rand() % 3;
		//		m_start = m_sSpawn[spawnPt];
		//		m_end = m_nodes[rand() % m_nodes.size()];
		//		soldier.setPosition(m_start->x, m_start->y);

		//		soldier.getBlackboard().set("HP", 5);
		//		soldier.getBlackboard().set("isAlive", true);
		//	}
		//	else {
		//		deathTimer += deltaTime;
			}
	//	}
	//}

	// update enemies
	for (auto& enemy : m_enemies) {
		bool alive;
		enemy.getBlackboard().get("isAlive", alive);
		if (alive) {
			int index;
			enemy.getBlackboard().get("targetIndex", index);
			enemy.update(&m_soldiers.at(index), deltaTime);
		//}
		////else {
		////	int deathTimer;
		//	enemy.getBlackboard().get("stateTimer", deathTimer);
		//	if (deathTimer > 5.0f) {
		//		unsigned int spawnPt = rand() % 10;
		//		enemy.setPosition(m_eSpawn[spawnPt].x, m_eSpawn[spawnPt].y);

		//		enemy.getBlackboard().set("HP", 3);
		//		enemy.getBlackboard().set("isAlive", true);
		//	}
		//	else {
		//		deathTimer += deltaTime;
		//	}
		}
	}


/*		float x = 0, y = 0;
		enemy.getPosition(&x, &y);

		// calculate sprite rotation
		float rotation = 0.0f; // rotation in radians
//		float rx = 0, ry = 0; 
		Vector2 *v = {};
		enemy.getBlackboard().get("velocity", &v);
		float vLength = sqrt(v->x*v->x + v->y*v->y);

		rx = v->x + x; // normalised x;
		ry = v->y + y; // normalised y;

		std::string str = std::to_string(v->x);
		const char* cstr = str.c_str();
		m_2dRenderer->drawText(m_font, cstr, x, y + 40);
		str = std::to_string(v->y);
		cstr = str.c_str();
		m_2dRenderer->drawText(m_font, cstr, x, y + 20);

		// draw enemy direction vector
		m_2dRenderer->setRenderColour(1, 1, 0);
		m_2dRenderer->drawLine(x, y, rx, ry);

		if (vLength > 0.0f) {
			if (v->x == 0) {
				if (v->y == 0) {
					// do nothing, not moving
				}
				else if (v->y > 0) {
					rotation = (float)M_PI_2;
				}
				else if (v->y < 0) {
					rotation = 3 * (float)M_PI_2;
				}
				else {}
			}
			else if (v->x > 0) {
				if (v->y == 0) {
					rotation = 0.0f;
				}
				else if (v->y > 0) {
					rotation = cosf(v->x / vLength);
				}
				else if (v->y < 0) {
					rotation = (3 * (float)M_PI_2) + cosf(v->y / vLength);
				}
				else {}
			}
			else if (v->x < 0) {
				if (v->y == 0) {
					rotation = (float)M_PI;
				}
				else if (v->y > 0) {
					rotation = (float)M_PI_2 + cosf(v->y / vLength);
				}
				else if (v->y < 0) {
					rotation = (float)M_PI + cosf(v->x / vLength);
				}
				else {}
			}
			else {}
		}
		else { enemy.getBlackboard().get("spriteRotation", rotation); }

		// update blackboard
		enemy.getBlackboard().set("spriteRotation", rotation);
	}
*/
	// input example
	aie::Input* input = aie::Input::getInstance();

/*	if (input->wasKeyPressed(aie::INPUT_KEY_I)) {
		m_steeringBehaviour.setWeightForForce(&m_idle, 0.6f);
		m_steeringBehaviour.setWeightForForce(&m_wander, 0.f);
	}

	if (input->wasKeyPressed(aie::INPUT_KEY_W)) {
		m_steeringBehaviour.setWeightForForce(&m_idle, 0.f);
		m_steeringBehaviour.setWeightForForce(&m_wander, 0.6f);
	}
*/
	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void AssessmentApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	float x = 0, y = 0;

	for (auto obstacle : m_boxObstacles) {
		//if (rayCircleIntersection(0, 0, // start of the line
		//	x, y, // direction of the line
		//	circle.x, circle.y, circle.r, // circle to collide with
		//	ix, iy)) { // point of collision

		//if (obstacle.type == Obstacle::SPHERE) {
		//	m_2dRenderer->setRenderColour(0, 1, 1);
		//	m_2dRenderer->drawCircle(obstacle.x, obstacle.y, obstacle.r);
		//}
		//
		//if (obstacle.type == Obstacle::BOX) {
			m_2dRenderer->setRenderColour(0, 1, 1);
			m_2dRenderer->drawBox(obstacle.x, obstacle.y, obstacle.w, obstacle.h);
		//}
	}

	for (auto node : m_nodes) {
/*		if (node == m_start) {
			m_2dRenderer->setRenderColour(0, 1, 0);
			m_2dRenderer->drawBox(node->x, node->y, 8, 8);
		}
		else if (node == m_end) {
			m_2dRenderer->setRenderColour(0, 0, 1);
			m_2dRenderer->drawBox(node->x, node->y, 8, 8);
		}
		else { */
			m_2dRenderer->setRenderColour(1, 0, 0);
			m_2dRenderer->drawBox(node->x, node->y, 4, 4);
//		}

		// draw edges
		m_2dRenderer->setRenderColour(1, 1, 0);
		for (auto edge : node->edges) {
			MyNode* target = (MyNode*)edge.target;

			m_2dRenderer->drawLine(node->x, node->y, target->x, target->y, 1, 1);
		}
	}

	// draw soldier
	int i = 0;
	for (auto& soldier : m_soldiers) {

		std::list<Pathfinding::Node*>* path = nullptr;
		soldier.getBlackboard().get("path", &path);
		// draw path
		m_2dRenderer->setRenderColour(0, 1, 0);
		for (auto node : *path) {

			MyNode* s = (MyNode*)node;
			MyNode* e = (MyNode*)node->previous;

			if (e != nullptr) {
				m_2dRenderer->drawLine(s->x, s->y, e->x, e->y, 3, i);
			}
		}

		m_2dRenderer->setRenderColour(1, 1, 1);
		soldier.getPosition(&x, &y);
		m_2dRenderer->drawCircle(x, y, 5);
/*		//m_2dRenderer->setUVRect((0 * w + 0 * pw), (11 * h + 11 * ph), w, h);
		m_2dRenderer->drawSprite(&m_charSpriteSheet, // reference to texture, passing nullptr draws a square
			x, y,
			20, 20, // width, height
			0, // rotation
			8 // depth
		); */

		int health;
		soldier.getBlackboard().get("HP", health);
		std::string str = std::to_string(health);
		const char* cstr = str.c_str();
		m_2dRenderer->drawText(m_font, cstr, x, y + 40);

		bool alive;
		soldier.getBlackboard().get("isAlive", alive);
		if (alive) {
			str = "alive";
		}
		else if (!alive) {
			str = "dead";
		}
		cstr = str.c_str();
		m_2dRenderer->drawText(m_font, cstr, x, y + 20);
		
		// draw enemy's detection radius
		m_2dRenderer->setRenderColour(1, 1, 0, 0.25f);
		m_2dRenderer->drawCircle(x, y, m_detectRadius, 10);

		++i;
	}
	
	// draw player as a green circle
	m_player.getPosition(&x, &y);
	m_2dRenderer->setRenderColour(0, 1, 0);
	m_2dRenderer->drawCircle(x, y, 5);

	screenWrap(x, y);
	m_player.setPosition(x, y);

	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	unsigned int seed = (unsigned int)timeinfo->tm_sec;

	//float i = 0;

	// draw the enemy as a red circle
	for (auto& enemy : m_enemies) {

		// sprite offsets for charSpriteSheet 
		float pw = 1.0f / m_spriteSheet.getWidth();
		float ph = 1.0f / m_spriteSheet.getHeight();
		float w = pw * 64.0f;
		float h = ph * 64.0f;

		enemy.getPosition(&x, &y);

		// draw enemy's detection radius
		m_2dRenderer->setRenderColour(1, 1, 0, 0.25f);
		m_2dRenderer->drawCircle(x, y, m_detectRadius, 10);

		float rx = 0, ry = 0; 
		Vector2 *v = {};
		enemy.getBlackboard().get("velocity", &v);

		rx = v->x + x; // normalised x;
		ry = v->y + y; // normalised y;

		int health;
		enemy.getBlackboard().get("HP", health);
		std::string str = std::to_string(health);
		const char* cstr = str.c_str();
		m_2dRenderer->drawText(m_font, cstr, x, y + 40);

		bool alive;
		enemy.getBlackboard().get("isAlive", alive);
		if (alive) {
			str = "alive";
		}
		else if (!alive) {
			str = "dead";
		}
		cstr = str.c_str();
		m_2dRenderer->drawText(m_font, cstr, x, y + 20);

		// draw enemy direction vector
		m_2dRenderer->setRenderColour(1, 1, 0);
		m_2dRenderer->drawLine(x, y, rx, ry);

		float rotation;
		enemy.getBlackboard().get("spriteRotation", rotation);

		//draw enemy sprite
		m_2dRenderer->setRenderColour(1, 0, 0);
		m_2dRenderer->drawCircle(x, y, 5);
/*		m_2dRenderer->setUVRect((8.0f * w + 8.0f * pw), (4.0f * h + 4.0f * ph), w, h);
		m_2dRenderer->drawSprite(&m_spriteSheet, // reference to texture, passing nullptr draws a square
			x, y,
			100, 100, // width, height
			0, // rotation
			0 // depth
		); */

		screenWrap(x, y);
		enemy.setPosition(x, y);
	}

/*	// demonstrate animation
	m_2dRenderer->setUVRect(int(m_timer) % 8 / 8.0f, 0, 1.f / 8, 1.f / 8);
	m_2dRenderer->drawSprite(m_texture, 200, 200, 100, 100);

	// demonstrate spinning sprite
	m_2dRenderer->setUVRect(0, 0, 1, 1);
	m_2dRenderer->drawSprite(m_shipTexture, 600, 400, 0, 0, m_timer, 1);

	// draw a thin line
	m_2dRenderer->drawLine(300, 300, 600, 400, 2, 1);

	// draw a moving purple circle
	m_2dRenderer->setRenderColour(1, 0, 1, 1);
	m_2dRenderer->drawCircle(sin(m_timer) * 100 + 600, 150, 50);

	// draw a rotating red box
	m_2dRenderer->setRenderColour(1, 0, 0, 1);
	m_2dRenderer->drawBox(600, 500, 60, 20, m_timer);

	// draw a slightly rotated sprite with no texture, coloured yellow
	m_2dRenderer->setRenderColour(1, 1, 0, 1);
	m_2dRenderer->drawSprite(nullptr, 400, 400, 50, 50, 3.14159f * 0.25f, 1);

	// output some text, uses the last used colour
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	m_2dRenderer->drawText(m_font, fps, 0, 720 - 32); */

	m_2dRenderer->setRenderColour(1, 1, 1);
	m_2dRenderer->drawText(m_font, "Press ESC to Quit", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}

void AssessmentApp::screenWrap(float & x, float & y)
{
	x = fmod(x, (float)getWindowWidth());
	if (x < 0)
		x += getWindowWidth();

	y = fmod(y, (float)getWindowHeight());
	if (y < 0)
		y += getWindowHeight();
}

