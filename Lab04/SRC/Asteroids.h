#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h"
#include "IPlayerListener.h"
#include "ScoreKeeper.h"
#include "Player.h"
#include "GUILabel.h"

class GameObject;
class Spaceship;
class GUILabel;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
	Asteroids(int argc, char *argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);

	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	virtual void OnScoreChanged(int score) {
		// Format the score message using an string-based stream 
		std::ostringstream msg_stream;
		msg_stream << "Score: " << score;
		// Get the score message as a string 
		std::string score_msg = msg_stream.str();
		mScoreLabel->SetText(score_msg);
	};

	void Asteroids::OnPlayerKilled(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);
private:
	ScoreKeeper mScoreKeeper;
	Player mPlayer;

	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<Spaceship> mSpaceship;

	uint mLevel;
	uint mAsteroidCount;

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	void CreateAsteroids(const uint num_asteroids);
	void CreateGUI();
	
	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;
};

#endif