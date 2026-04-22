#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "GameObject.h"

class PowerUp : public GameObject
{
public:
	enum PowerUpType
	{
		EXTRA_LIFE = 0,
		INVULNERABILITY = 1,
		BRAKES = 2
	};

	PowerUp(PowerUpType type, int ttl_ms = 10000);
	virtual ~PowerUp(void);

	virtual void Update(int t);
	virtual bool CollisionTest(shared_ptr<GameObject> o);
	virtual void OnCollision(const GameObjectList& objects);

	PowerUpType GetPowerUpType() const { return mPowerUpType; }
	bool WasCollected() const { return mCollected; }

private:
	PowerUpType mPowerUpType;
	int mTimeToLive;
	bool mCollected;
};

#endif
