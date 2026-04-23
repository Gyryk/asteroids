#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "GUIIcon.h"
#include "Explosion.h"
#include "Bonus.h"
#include "ImageManager.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv)
{
	mLevel = 0;
	mAsteroidCount = 0;
	mBonusHeld = HELD_NONE;
	mShieldImage = NULL;
	mNukeImage = NULL;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation *asteroid2_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid2", 128, 8192, 128, 128, "asteroid2_fs.png");
	Animation *asteroid3_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid3", 64, 4096, 64, 64, "asteroid3_fs.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");
	Animation* life_anim = AnimationManager::GetInstance().CreateAnimationFromFile("life_powerup", 512, 512, 512, 512, "life.png");
	Animation* shield_anim = AnimationManager::GetInstance().CreateAnimationFromFile("shield_powerup", 512, 512, 512, 512, "shield.png");
	Animation* nuke_anim = AnimationManager::GetInstance().CreateAnimationFromFile("nuke_powerup", 512, 512, 512, 512, "nuke.png");
	mShieldImage = ImageManager::GetInstance().CreateImageFromFile("shield_icon", 512, 512, "shield.png");
	mNukeImage = ImageManager::GetInstance().CreateImageFromFile("nuke_icon", 512, 512, "nuke.png");

	// Create a spaceship and add it to the world
	mGameWorld->AddObject(CreateSpaceship());
	// Create some asteroids and add them to the world
	CreateAsteroids(10);

	//Create the GUI
	CreateGUI();

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	switch (key)
	{
	case ' ':
		mSpaceship->Shoot();
		break;
	case 'c':
	case 'C':
		if (mBonusHeld == HELD_INVULNERABILITY)
		{
			mSpaceship->SetInvulnerableFor(5000);
			mBonusHeld = HELD_NONE;
			UpdateBonusHeldIcon();
			// Show text for invulnerability timer. WIP
			// mInvulnerabilityLabel->SetVisible(true);
			// SetTimer(5000, HIDE_INVULNERABILITY_TEXT);
		}
		else if (mBonusHeld == HELD_NUKE)
		{
			ActivateNuke();
			mBonusHeld = HELD_NONE;
			UpdateBonusHeldIcon();
		}
		break;
	default:
		break;
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is pressed start applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
	// If left arrow key is pressed start rotating anti-clockwise
	case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
	// If right arrow key is pressed start rotating clockwise
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
	// Default case - do nothing
	default: break;
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is released stop applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
	// If left arrow key is released stop rotating
	case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
	// If right arrow key is released stop rotating
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
	// Default case - do nothing
	default: break;
	} 
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		shared_ptr<Asteroid> asteroid = dynamic_pointer_cast<Asteroid>(object);
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		if (asteroid.get() != NULL && asteroid->GetSize() == Asteroid::LARGE && asteroid->WasDestroyedByBullet())
		{
			bool should_split = (rand() % 100) < 40; // 40% chance of splitting into smaller asteroids
			asteroid->SetSplitOnDestroy(should_split);
			if (should_split)
			{
				CreateSmallAsteroids(object->GetPosition(), object->GetVelocity());
				mAsteroidCount += 2;
			}

			bool drop_bonus = (rand() % 100) < 99; // 20% chance of dropping a bonus
			if (drop_bonus)
			{
				SpawnBonus(object->GetPosition());
			}
		}
		if (mAsteroidCount <= 0) 
		{ 
			SetTimer(500, START_NEXT_LEVEL); 
		}
	}
	
	if (object->GetType() == GameObjectType("Bonus"))
	{
		shared_ptr<Bonus> bonus = dynamic_pointer_cast<Bonus>(object);
		if (bonus.get() != NULL && bonus->WasCollected())
		{
			if (bonus->GetBonusType() == Bonus::EXTRA_LIFE)
			{
				mPlayer.AddLife();
				std::ostringstream msg_stream;
				msg_stream << "Lives: " << mPlayer.GetLives();
				mLivesLabel->SetText(msg_stream.str());
			}
			else if (bonus->GetBonusType() == Bonus::INVULNERABILITY)
			{
				if (mBonusHeld == HELD_NONE)
				{
					mBonusHeld = HELD_INVULNERABILITY;
					UpdateBonusHeldIcon();
				}
			}
			else if (bonus->GetBonusType() == Bonus::NUKE)
			{
				if (mBonusHeld == HELD_NONE)
				{
					mBonusHeld = HELD_NUKE;
					UpdateBonusHeldIcon();
				}
			}
		}
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 10 + 5 * mLevel;
		CreateAsteroids(num_asteroids);
		SpawnBonus(GLVector3f(0.0f, 0.0f, 0.0f));
	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
	}

	if (value == HIDE_INVULNERABILITY_TEXT)
	{
		mInvulnerabilityLabel->SetVisible(false);
	}
}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		string asteroid_name = (rand() % 2 == 0) ? "asteroid1" : "asteroid2";
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName(asteroid_name);
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);
	}
}

void Asteroids::CreateSmallAsteroids(const GLVector3f& origin, const GLVector3f& inherited_velocity)
{
	for (int i = 0; i < 2; ++i)
	{
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid3");
		shared_ptr<Sprite> asteroid_sprite =
			make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<Asteroid> asteroid = make_shared<Asteroid>(Asteroid::SMALL);
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 5.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.12f);
		asteroid->SetPosition(origin);
		GLVector3f spread((i == 0) ? 8.0f : -8.0f, (i == 0) ? -8.0f : 8.0f, 0.0f);
		asteroid->SetVelocity(inherited_velocity + spread);
		mGameWorld->AddObject(asteroid);
	}
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));

	mBonusHeldIcon = make_shared<GUIIcon>();
	mBonusHeldIcon->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_RIGHT);
	mBonusHeldIcon->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	mBonusHeldIcon->SetVisible(false);
	mBonusHeldIcon->SetSize(GLVector2i(32, 32));
	shared_ptr<GUIComponent> held_bonus_component
		= static_pointer_cast<GUIComponent>(mBonusHeldIcon);
	mGameDisplay->GetContainer()->AddComponent(held_bonus_component, GLVector2f(1.0f, 0.0f));

	mInvulnerabilityLabel = make_shared<GUILabel>("Invulnerability: 5000");
	mInvulnerabilityLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mInvulnerabilityLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_RIGHT);
	mInvulnerabilityLabel->SetVisible(false);
	shared_ptr<GUIComponent> invulnerability_component = static_pointer_cast<GUIComponent>(mInvulnerabilityLabel);
	mGameDisplay->GetContainer()->AddComponent(invulnerability_component, GLVector2f(1.0f, 1.0f));
}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER); 
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}

void Asteroids::SpawnBonus(const GLVector3f& position)
{
	int probability = rand() % 100;
	Bonus::BonusType random_type;
	if(probability < 30)
	{
		random_type = Bonus::EXTRA_LIFE;
	}
	else if(probability < 90)
	{
		random_type = Bonus::INVULNERABILITY;
	}
	else
	{
		random_type = Bonus::NUKE;
	}

	shared_ptr<Bonus> bonus = make_shared<Bonus>(random_type, 70000);
	bonus->SetBoundingShape(make_shared<BoundingSphere>(bonus->GetThisPtr(), 8.0f));
	string animation_name = "life_powerup";
	if (random_type == Bonus::INVULNERABILITY) animation_name = "shield_powerup";
	if (random_type == Bonus::NUKE) animation_name = "nuke_powerup";
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName(animation_name);
	shared_ptr<Sprite> bonus_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	bonus_sprite->SetLoopAnimation(true);
	bonus->SetSprite(bonus_sprite);
	bonus->SetScale(-0.03f);
	bonus->SetPosition(position);
	mGameWorld->AddObject(bonus);
}

void Asteroids::ActivateNuke()
{
	GameObjectList objects = mGameWorld->GetGameObjects();
	for (GameObjectList::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if ((*it)->GetType() == GameObjectType("Asteroid"))
		{
			shared_ptr<Asteroid> asteroid = dynamic_pointer_cast<Asteroid>(*it);
			if (asteroid.get() != NULL)
			{
				asteroid->SetDestroyedByBullet(true);
			}
			mGameWorld->FlagForRemoval(*it);
		}
	}
}

void Asteroids::UpdateBonusHeldIcon()
{
	if (mBonusHeldIcon.get() == NULL) return;
	if (mBonusHeld == HELD_INVULNERABILITY)
	{
		mBonusHeldIcon->SetImage(mShieldImage);
		mBonusHeldIcon->SetVisible(true);
	}
	else if (mBonusHeld == HELD_NUKE)
	{
		mBonusHeldIcon->SetImage(mNukeImage);
		mBonusHeldIcon->SetVisible(true);
	}
	else
	{
		mBonusHeldIcon->SetVisible(false);
	}
}