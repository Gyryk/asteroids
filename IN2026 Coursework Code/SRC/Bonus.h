#pragma once
#ifndef __BONUS_H__
#define __BONUS_H__

#include "GameObject.h"

class Bonus : public GameObject
{
public:
	enum BonusType
	{
		EXTRA_LIFE = 0,
		INVULNERABILITY = 1,
		NUKE = 2
	};

	Bonus(BonusType type, int ttl_ms = 10000);
	virtual ~Bonus(void);

	virtual void Update(int t);
	virtual bool CollisionTest(shared_ptr<GameObject> o);
	virtual void OnCollision(const GameObjectList& objects);

	BonusType GetBonusType() const { return mBonusType; }
	bool WasCollected() const { return mCollected; }

private:
	BonusType mBonusType;
	int mTimeToLive;
	bool mCollected;
};

#endif