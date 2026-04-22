#include <stdlib.h>
#include "PowerUp.h"
#include "GameObjectType.h"

PowerUp::PowerUp(PowerUpType type, int ttl_ms)
	: GameObject("PowerUp"),
	  mPowerUpType(type),
	  mTimeToLive(ttl_ms),
	  mCollected(false)
{
	mPosition.x = (rand() % 180) - 90.0f;
	mPosition.y = (rand() % 180) - 90.0f;
	mPosition.z = 0.0f;
	mVelocity = GLVector3f(0.0f, 0.0f, 0.0f);
}

PowerUp::~PowerUp(void)
{
}

void PowerUp::Update(int t)
{
	GameObject::Update(t);
	mTimeToLive -= t;
	if (mTimeToLive <= 0 && mWorld)
	{
		mWorld->FlagForRemoval(GetThisPtr());
	}
}

bool PowerUp::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Spaceship")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void PowerUp::OnCollision(const GameObjectList& objects)
{
	for (GameObjectList::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if ((*it)->GetType() == GameObjectType("Spaceship"))
		{
			mCollected = true;
			break;
		}
	}
	mWorld->FlagForRemoval(GetThisPtr());
}
