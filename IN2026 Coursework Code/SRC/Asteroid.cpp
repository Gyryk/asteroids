#include <stdlib.h>
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"
#include "Bullet.h"
#include "Spaceship.h"

namespace
{
	const float LARGE_BOUNCINESS = 0.9f;
	const float SMALL_BOUNCINESS = 1.0f;
}

Asteroid::Asteroid(void) : GameObject("Asteroid"), mSize(LARGE), mDestroyedByBullet(false), mSplitOnDestroy(false)
{
	mAngle = rand() % 360;
	mRotation = 0; // rand() % 90;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 10.0 * cos(DEG2RAD*mAngle);
	mVelocity.y = 10.0 * sin(DEG2RAD*mAngle);
	mVelocity.z = 0.0;
}

Asteroid::Asteroid(AsteroidSize size) : GameObject("Asteroid"), mSize(size), mDestroyedByBullet(false), mSplitOnDestroy(false)
{
	mAngle = rand() % 360;
	mRotation = 0;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	float base_speed = (mSize == LARGE) ? 10.0f : 16.0f;
	mVelocity.x = base_speed * cos(DEG2RAD * mAngle);
	mVelocity.y = base_speed * sin(DEG2RAD * mAngle);
	mVelocity.z = 0.0;
}

Asteroid::~Asteroid(void)
{
}

bool Asteroid::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() == GameObjectType("Explosion")) return false;
	if (o->GetType() == GameObjectType("PowerUp")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Asteroid::OnCollision(const GameObjectList& objects)
{
	for (GameObjectList::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		shared_ptr<GameObject> other = *it;
		if (other->GetType() == GameObjectType("Asteroid"))
		{
			// This prevents double processing
			// if (GetThisPtr().get() < other.get())
			// {
				Bounce(other);
			// }
		}
		else if (other->GetType() == GameObjectType("Bullet"))
		{
			mDestroyedByBullet = true;
			mWorld->FlagForRemoval(GetThisPtr());
			return;
		}
		else if (other->GetType() == GameObjectType("Spaceship"))
		{
			if (mSize == LARGE)
			{
				mWorld->FlagForRemoval(GetThisPtr());
				return;
			}
			Bounce(other);
		}
	}
}

int Asteroid::GetScoreValue() const
{
	if (mSize == LARGE)
	{
		return mSplitOnDestroy ? 15 : 20;
	}
	return 10;
}

void Asteroid::Bounce(shared_ptr<GameObject> o)
{
	GLVector3f delta = mPosition - o->GetPosition();
	delta.z = 0.0f;
	if (delta.length() == 0) delta = GLVector3f(1.0f, 0.0f, 0.0f);
	delta.normalize();

	GLVector3f this_velocity = mVelocity;
	GLVector3f other_velocity = o->GetVelocity();

	GLfloat this_normal_speed = this_velocity.dot(delta);
	GLfloat other_normal_speed = other_velocity.dot(delta);

	GLVector3f this_normal = delta * this_normal_speed;
	GLVector3f other_normal = delta * other_normal_speed;
	GLVector3f this_tangent = this_velocity - this_normal;
	GLVector3f other_tangent = other_velocity - other_normal;

	const float BOUNCINESS = (mSize == LARGE) ? LARGE_BOUNCINESS : SMALL_BOUNCINESS;
	GLVector3f this_new_velocity = (other_normal + this_tangent) * BOUNCINESS;
	GLVector3f other_new_velocity = (this_normal + other_tangent) * BOUNCINESS;

	mPosition += delta * 0.2f;
	o->SetPosition(o->GetPosition() - delta * 0.2f);
	mVelocity = this_new_velocity;
	o->SetVelocity(other_new_velocity);
}
