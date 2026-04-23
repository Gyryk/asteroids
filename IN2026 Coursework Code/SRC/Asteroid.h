#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "GameObject.h"

class Asteroid : public GameObject
{
public:
	enum AsteroidSize
	{
		LARGE = 0,
		SMALL = 1
	};

	Asteroid(void);
	Asteroid(AsteroidSize size);

	~Asteroid(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);

	AsteroidSize GetSize() const { return mSize; }
	void SetSize(AsteroidSize size) { mSize = size; }
	bool WasDestroyedByBullet() const { return mDestroyedByBullet; }
	void SetDestroyedByBullet(bool destroyed_by_bullet) { mDestroyedByBullet = destroyed_by_bullet; }
	void SetSplitOnDestroy(bool split_on_destroy) { mSplitOnDestroy = split_on_destroy; }
	bool WasSplitOnDestroy() const { return mSplitOnDestroy; }
	int GetScoreValue() const;

private:
	AsteroidSize mSize;
	bool mDestroyedByBullet;
	bool mSplitOnDestroy;

	void Bounce(shared_ptr<GameObject> o);
};

#endif
