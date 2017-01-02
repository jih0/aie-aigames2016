#include "SteeringBehaviour.h"
#include "aiUtilities.h"

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

	return { xDiff * gameObject->getMaxForce(), yDiff * gameObject->getMaxForce() };
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

	return{ xDiff * gameObject->getMaxForce(), yDiff * gameObject->getMaxForce() };
}

Force PursueForce::getForce(GameObject* gameObject) const {

	// get target position
	float tx = 0, ty = 0;
	m_target->getPosition(&tx, &ty);
	
	// get target velocity
	float tvx = 0, tvy = 0;
	m_target->getVelocity(&tvx, &tvy);

	// add velocity to target
	tx += tvx;
	ty += tvy;

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

	return{ xDiff * gameObject->getMaxForce(), yDiff * gameObject->getMaxForce() };
}

Force EvadeForce::getForce(GameObject* gameObject) const {

	// get target position
	float tx = 0, ty = 0;
	m_target->getPosition(&tx, &ty);

	// get target velocity
	float tvx = 0, tvy = 0;
	m_target->getVelocity(&tvx, &tvy);

	// add velocity to target
	tx += tvx;
	ty += tvy;

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

	return{ xDiff * gameObject->getMaxForce(), yDiff * gameObject->getMaxForce() };
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
	
	glm::vec2 jitterOffset = glm::circularRand(gameObject->getWanderJitter());

	float wanderX = 0, wanderY = 0;
	gameObject->getWanderTarget(&wanderX, &wanderY);

	// applied the jitter
	wanderX += jitterOffset.x;
	wanderY += jitterOffset.y;

	float magnitude = sqrt(wanderX * wanderX + wanderY * wanderY);

	// bring it back to a radius around the game object
	wanderX = wanderX / magnitude * gameObject->getWanderRadius();
	wanderY = wanderY / magnitude * gameObject->getWanderRadius();

	// store the target back into the game object
	gameObject->setWanderTarget(wanderX, wanderY);

	// access the game object's velocity as a unit vector
	float vx = 0, vy = 0;
	gameObject->getVelocity(&vx, &vy);

	// normalise and protect from divide by 0 error
	magnitude = vx * vx + vy * vy;
	if (magnitude > 0) {
		magnitude = sqrt(magnitude);
		vx /= magnitude;
		vy /= magnitude;
	}

	// combine velocity direction with wanter target to offset
	wanderX += vx * gameObject->getWanderOffset();
	wanderY += vy * gameObject->getWanderOffset();

	// normalise the new direction
	magnitude = wanderX * wanderX + wanderY * wanderY;
	if (magnitude > 0) {

		magnitude = sqrt(magnitude);
		wanderX /= magnitude;
		wanderY /= magnitude;
	}

	return{ wanderX * gameObject->getMaxForce(), wanderY * gameObject->getMaxForce() };

}

Force ObstacleAvoidanceForce::getForce(GameObject* gameObject) const {

	Force force = {};

	// create feeler
	float x, y, vx, vy;
	gameObject->getPosition(&x, &y);
	gameObject->getVelocity(&vx, &vy);

	float ix, iy, t;

	// are we moving?
	float magSqr = vx * vx + vy * vy;
	if (magSqr > 0) {

		// loop through all obstacles and find collisions
		for (auto& obstacle : m_obstacles) {
			if (rayCircleIntersection(	x, y,
										vx, vy,
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
		}
	}

	// normalise force
	magSqr = force.x * force.x + force.y * force.y;
	if (magSqr > 0) {
		magSqr = sqrt(magSqr);
		force.x /= magSqr;
		force.y /= magSqr;
	}

	return{ force.x * gameObject->getMaxForce(), force.y * gameObject->getMaxForce() };
}