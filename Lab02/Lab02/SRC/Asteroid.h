#pragma once
#ifndef _ASTEROID_H_
#define _ASTEROID_H_
#include "GameObject.h"

class Asteroid : public GameObject
{
public:
	Asteroid(void);
	~Asteroid(void);
	void Render(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};

#endif