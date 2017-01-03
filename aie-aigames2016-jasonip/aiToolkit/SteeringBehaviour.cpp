#include "SteeringBehaviour.h"
#include "aiUtilities.h"
#include "Blackboard.h"

#include <glm/ext.hpp>

Force SeekForce::getForce(GameObject* gameObject) const {

	// get target position
	float tx = 0, ty = 0;
	m_target->getPosition(&tx, &ty);

	// get my position
	float x = 0, y = 0;
	gameObject->getPosition(&x, &y);

	// get a vector to the target from "us"
	float xDiff = tx - x;
	float yDiff = ty - y;
	float distance = (xDiff * xDiff + yDiff * yDiff);

	// if not at the target then move towards them
	if (distance > 0) {

		distance = sqrt(distance);

		// need to make the difference the length of 1 (normalize)
		// this is so movement can be "pixels per second"
		xDiff /= distance;
		yDiff /= distance;
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return { xDiff * maxForce, yDiff * maxForce };
}

Force FleeForce::getForce(GameObject* gameObject) const {

	// get target position
	float tx = 0, ty = 0;
	m_target->getPosition(&tx, &ty);

	// get my position
	float x = 0, y = 0;
	gameObject->getPosition(&x, &y);

	// get a vector from "us" to the target
	float xDiff = x - tx;
	float yDiff = y - ty;
	float distance = (xDiff * xDiff + yDiff * yDiff);

	// if not at the target then move towards them
	if (distance > 0) {

		distance = sqrt(distance);

		// need to make the difference the length of 1 (normalize)
		// this is so movement can be "pixels per second"
		xDiff /= distance;
		yDiff /= distance;
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ xDiff * maxForce, yDiff * maxForce };
}

Force PursueForce::getForce(GameObject* gameObject) const {

	// get target position
	float tx = 0, ty = 0;
	m_target->getPosition(&tx, &ty);
	
	// get target velocity
	Vector2* velocity = nullptr;
	gameObject->getBlackboard().get("velocity", &velocity);

	// add velocity to target
	tx += velocity->x;
	ty += velocity->y;

	// get my position
	float x = 0, y = 0;
	gameObject->getPosition(&x, &y);

	// get a vector to the target from "us"
	float xDiff = tx - x;
	float yDiff = ty - y;
	float distance = (xDiff * xDiff + yDiff * yDiff);

	// if not at the target then move towards them
	if (distance > 0) {

		distance = sqrt(distance);

		// need to make the difference the length of 1 (normalize)
		// this is so movement can be "pixels per second"
		xDiff /= distance;
		yDiff /= distance;
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ xDiff * maxForce, yDiff * maxForce };
}

Force EvadeForce::getForce(GameObject* gameObject) const {

	// get target position
	float tx = 0, ty = 0;
	m_target->getPosition(&tx, &ty);

	// get target velocity
	Vector2* velocity = nullptr;
	gameObject->getBlackboard().get("velocity", &velocity);

	// add velocity to target
	tx += velocity->x;
	ty += velocity->y;

	// get my position
	float x = 0, y = 0;
	gameObject->getPosition(&x, &y);

	// get a vector from "us" to the target
	float xDiff = x - tx;
	float yDiff = y - ty;
	float distance = (xDiff * xDiff + yDiff * yDiff);

	// if not at the target then move towards them
	if (distance > 0) {

		distance = sqrt(distance);

		// need to make the difference the length of 1 (normalize)
		// this is so movement can be "pixels per second"
		xDiff /= distance;
		yDiff /= distance;
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ xDiff * maxForce, yDiff * maxForce };
}

// possible glm implementation
// glm::vec2 targetPos = m_target->getPosition();
// glm::vec2 myPos = gameObject->getPosition();
//
// glm::vec2 direction = targetPos - myPos;
// if (glm::length(direction) > 0) {
//
// }

Force WanderForce::getForce(GameObject* gameObject) const {
	
	WanderData* wd = nullptr;
	if (gameObject->getBlackboard().get("wanderData", &wd) == false) {
		return{ 0, 0 };
	}

	glm::vec2 jitterOffset = glm::circularRand(wd->jitter);

	float wanderX = wd->x, wanderY = wd->y;

	// applied the jitter
	wanderX += jitterOffset.x;
	wanderY += jitterOffset.y;

	float magnitude = sqrt(wanderX * wanderX + wanderY * wanderY);

	// bring it back to a radius around the game object
	wanderX = wanderX / magnitude * wd->radius;
	wanderY = wanderY / magnitude * wd->radius;

	// store the target back into the game object
	wd->x = wanderX;
	wd->y = wanderY;

	// access the game object's velocity as a unit vector
	Vector2* velocity = nullptr;
	gameObject->getBlackboard().get("velocity", &velocity);
	float vx = velocity->x;
	float vy = velocity->y;

	// normalise and protect from divide by 0 error
	magnitude = vx * vx + vy * vy;
	if (magnitude > 0) {
		magnitude = sqrt(magnitude);
		vx /= magnitude;
		vy /= magnitude;
	}

	// combine velocity direction with wanter target to offset
	wanderX += vx * wd->offset;
	wanderY += vy * wd->offset;

	// normalise the new direction
	magnitude = wanderX * wanderX + wanderY * wanderY;
	if (magnitude > 0) {

		magnitude = sqrt(magnitude);
		wanderX /= magnitude;
		wanderY /= magnitude;
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ wanderX * maxForce, wanderY * maxForce };

}

Force ObstacleAvoidanceForce::getForce(GameObject* gameObject) const {

	Force force = {};

	// create feeler
	float x, y;
	gameObject->getPosition(&x, &y);
	Vector2* velocity = nullptr;
	gameObject->getBlackboard().get("velocity", &velocity);


	float ix, iy, t;

	// are we moving?
	float magSqr = velocity->x * velocity->x + velocity->y * velocity->y;
	if (magSqr > 0) {

		// loop through all obstacles and find collisions
		for (auto& obstacle : m_obstacles) {
			if (rayCircleIntersection(	x, y,
										velocity->x, velocity->y,
										obstacle.x, obstacle.y, obstacle.r,
										ix, iy,
										&t)) {
				// is the collision within range? where t is the distance to collision
				if (t >= 0 &&
					t <= m_feelerLength) {

					// get direction vector and make it a unit vector by dividing by the circle obstacle's radius
					force.x += (ix - obstacle.x) / obstacle.r;
					force.y += (iy - obstacle.y) / obstacle.r;
				}
			}

			// rotate feeler about 30 degrees
			float s = sinf(3.14159f*0.15f);
			float c = cosf(3.14159f*0.15f);
			if (rayCircleIntersection(	x, y,
										velocity->x * c - velocity->y * s, velocity->x * s + velocity->y * c, // apply rotation to vector
										obstacle.x, obstacle.y, obstacle.r,
										ix, iy, &t)) {
				if (t >= 0 &&
					t <= m_feelerLength * 0.5f) { // scale feeler 50%
					force.x += (ix - obstacle.x) / obstacle.r;
					force.y += (iy - obstacle.y) / obstacle.r;
				}
			}

			// rotate feeler about -30 degrees
			s = sinf(3.14159f*-0.15f);
			c = cosf(3.14159f*-0.15f);
			if (rayCircleIntersection(	x, y,
										velocity->x * c - velocity->y * s, velocity->x * s + velocity->y * c, // apply rotation to vector
										obstacle.x, obstacle.y, obstacle.r,
										ix, iy, &t)) {
				if (t >= 0 &&
					t <= m_feelerLength * 0.5f) { // scale feeler 50%
					force.x += (ix - obstacle.x) / obstacle.r;
					force.y += (iy - obstacle.y) / obstacle.r;
				}
			}

		}
	}

	// normalise force
	magSqr = force.x * force.x + force.y * force.y;
	if (magSqr > 0) {
		magSqr = sqrt(magSqr);
		force.x /= magSqr;
		force.y /= magSqr;
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ force.x * maxForce, force.y * maxForce };
}