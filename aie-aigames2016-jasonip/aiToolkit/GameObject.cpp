#include "GameObject.h"
#include "Behaviour.h"

#include <imgui.h>

GameObject::GameObject()
	: m_x(0),
	m_y(0) {
}

GameObject::~GameObject()
{
}

void GameObject::addBehaviour(Behaviour * behaviour)
{
	m_behaviours.push_back(behaviour);
}

void GameObject::update(float deltaTime)
{
	// execute all behaviours
	for (auto behaviour : m_behaviours)
		behaviour->execute(this, deltaTime);
}

void GameObject::update(GameObject* target, float deltaTime)
{
	// execute all behaviours
	for (auto behaviour : m_behaviours)
		behaviour->execute(this, deltaTime);

	// poll health of gameObject
	int health;
	this->getBlackboard().get("HP", health);

	if (health <= 0) {
		this->getBlackboard().set("attacking", false);
		this->getBlackboard().set("isAlive", false);
	}
	else {
		// if still alive, attack!
		bool attack;
		this->getBlackboard().get("attacking", attack);
		if (attack) {
			float cooldown;
			this->getBlackboard().get("attackCooldown", cooldown);

			float tx, ty;
			target->getPosition(&tx, &ty);

			float x, y;
			this->getPosition(&x, &y);

			float distance = sqrt((tx - x) * (tx - x) + (ty - y) * (ty - y));
			float range;
			this->getBlackboard().get("attackRange", range);

			//  (... if the target is in gameObject's attack range)
			if (distance < range) {

				if (cooldown <= 0.0f) {
					int tHP, AP;
					target->getBlackboard().get("HP", tHP);
					this->getBlackboard().get("AP", AP);
					tHP -= AP;
					target->getBlackboard().set("HP", tHP);
					this->getBlackboard().set("attackCooldown", 1.0f);
				}
				else {
					cooldown -= deltaTime;
					this->getBlackboard().set("attackCooldown", cooldown);
				}
			}
		}
	}
}
