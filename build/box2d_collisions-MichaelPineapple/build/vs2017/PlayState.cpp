#include "Nexus.h"

// Total time alloted
const float STARTING_TIME = 50.0f;

// current difficulty
int difficulty = 1;

// player's movement speed
const float playerMovementSpeed = 0.25f;

// horizontal velocity of the player
float playervel = 0.0f;

// player object
MclObject* player;

// user's score
int score = 0;

// keep track of the remaining time
float ramainingtime = STARTING_TIME;

// is the game over?
bool gameOver = false;

// text positioning variables
float centerX, centerY;

/* Modify the score

_p - Points to add
*/
void modPoints(int _p)
{
	if (!gameOver) // do not modify score if game over
	{
		// if score is being increased, play the good sound, if being decreased play the bad sound
		if (_p > 0) Nexus::PLAY_AUDIO(Nexus::SOUND_GOOD);
		else Nexus::PLAY_AUDIO(Nexus::SOUND_BAD);

		// modify the score
		score += _p;
	}
}

/* Returns a random position above the screen, used for spawning new skulls */
b2Vec2 getRndBallStart()
{
	return b2Vec2(Nexus::RND(-3, 3), Nexus::RND(10, 20));
}

/* OnCollide callback, called when two Box2D bodies collide

_bodyA - One of the two bodies in collision
_bodyB - The other body in collision
*/
void onCollide(b2Body* _bodyA, b2Body* _bodyB)
{
	// get 'GameObjects' from 'Bodys'
	GameObject* objA = (GameObject*)_bodyA->GetUserData();
	GameObject* objB = (GameObject*)_bodyB->GetUserData();

	if (objA != NULL && objB != NULL) // check that objects are not null
	{
		if (objA->collideCooldown > 1 || objB->collideCooldown > 1) // check that these bodies have not recently collided
		{
			// Get entity types of the colliding objects
			ObjectType typeA = objA->type_;
			ObjectType typeB = objB->type_;

			if (typeA != typeB)  // check the two objects are not the same type of entity
			{
				if (typeB == ENEMY) // check if object B is of type ENEMY
				{
					if (typeA == PLAYER)
					{
						// if object A is type PLAYER, bounce object B and increase score by 1
						_bodyB->ApplyForce(b2Vec2(playervel, 70.0f), _bodyB->GetPosition(), true);
						modPoints(1);
					}
					else if (typeA == LEFT_WALL)
					{
						// if object A is a LEFT_WALL, bounce object B right
						_bodyB->ApplyForce(b2Vec2(10.0f, 0.0f), _bodyB->GetPosition(), true);
					}
					else if (typeA == RIGHT_WALL)
					{
						// if object A is a RIGHT_WALL, bounce object B left
						_bodyB->ApplyForce(b2Vec2(-10.0f, 0.0f), _bodyB->GetPosition(), true);
					}
				}
			}

			// play impact sound effect
			Nexus::PLAY_AUDIO(Nexus::GET_IMPACT_SOUND());
		}

		// reset collision cooldown timers for both objects
		objA->collideCooldown = 0;
		objB->collideCooldown = 0;
	}
}


/* Initialize the PLAY game state

_platform - Application platform
*/
void PlayState::INIT(Platform& _platform)
{
	// intialize the physics simulation
	Simulation::INITIALIZE(_platform, -3.0f);

	// intialize some values
	gameOver = false;
	ramainingtime = STARTING_TIME;
	score = 0;

	// get some text positioning data from the screen size
	centerX = _platform.width() / 2.0f;
	centerY = _platform.height() / 2.0f;

	// play music
	Nexus::PLAY_MUSIC();

	// get number of skulls from difficulty
	int ballsnum = 0;
	switch (difficulty)
	{
		case 0: ballsnum = 3; break;
		case 1: ballsnum = 6; break;
		case 2: ballsnum = 12; break;
		default: ballsnum = 0; break;
	}

	// Create left and right walls to keep skulls from moving off the screen
	MclObject* leftWall = Simulation::CREATE_ENTITY(b2Vec2(-7.6f, 0.0f), b2Vec2(1.0f, 200.0f), false, LEFT_WALL);
	MclObject* rightWall = Simulation::CREATE_ENTITY(b2Vec2(7.6f, 0.0f), b2Vec2(1.0f, 200.0f), false, RIGHT_WALL);
	leftWall->setVisible(false);
	rightWall->setVisible(false);
	Simulation::ADD_ENTITY(leftWall);
	Simulation::ADD_ENTITY(rightWall);

	// Create the player
	player = Simulation::CREATE_ENTITY(b2Vec2(0.0f, 0.5f), b2Vec2(2.5f, 0.8f), false, PLAYER, Nexus::MESH_PILLOW);
	Simulation::ADD_ENTITY(player);

	// create skulls above the screen at random positions and rotations
	for (int i = 0; i < ballsnum; i++)
	{
		MclObject* skulltmp = Simulation::CREATE_ENTITY(getRndBallStart(), b2Vec2(0.4f, 0.5f), true, ENEMY, Nexus::MESH_SKULL);
		skulltmp->getBody()->SetAngularVelocity(Nexus::RND(-1000, 1000)/100.0f);
		Simulation::ADD_ENTITY(skulltmp);
	}
}

/* Update the PLAY game state

_kb - Keyboard input data
_platform - Application platform
_frameTime - Elapsed time on the last frame
*/
void PlayState::UPDATE(Keyboard* _kb, Platform& _platform, float _frameTime)
{
	// keep track of how much time has elapsed, if time is up, set 'gameOver' true
	if (!gameOver)
	{
		ramainingtime -= _frameTime;
		if (ramainingtime <= 0)
		{
			gameOver = true;
			Nexus::STOP_MUSIC();
		}
	}

	// Update the phyics simulation, pass it the onCollide callback
	Simulation::UPDATE(onCollide);

	// If a skull falls below the screen, decrease the score by 5 and reset the skull
	vector<MclObject*> entList = Simulation::GET_ENTITIES();
	for (int i = 0; i < entList.size(); i++)
	{
		MclObject* ent = entList[i];
		if (ent->getPos().y < -1.0f)
		{
			modPoints(-5);
			ent->setPos(getRndBallStart());
			ent->setVel(b2Vec2_zero);
		}
	}

	// Control the player. LEFT key moves the pillow left and RIGHT key moves it right. keep track of the player's horizontal velocity
	playervel = 0.0f;
	if (_kb->IsKeyDown(_kb->KC_LEFT))
	{
		if (player->getPos().x > -6.0f) player->move(b2Vec2(-playerMovementSpeed, 0.0f));
		playervel = -10.0f;
	}
	else if (_kb->IsKeyDown(_kb->KC_RIGHT))
	{
		if (player->getPos().x < 6.0f) player->move(b2Vec2(playerMovementSpeed, 0.0f));
		playervel = 10.0f;
	}

	// If user presses ESC, stop music and go to the MENU gamestate
	if (_kb->IsKeyDown(_kb->KC_ESCAPE))
	{
		Nexus::STOP_MUSIC();
		Nexus::CHANGE_STATE(MENU, _platform);
	}
}

/* Render 3D meshes for the PLAY game state */
void PlayState::RENDER()
{
	Simulation::RENDER();
}

/* Render 2D bitmaps for the PLAY game state */
void PlayState::RENDER_HUD()
{
	// display score
	Nexus::DRAW_TXT("SCORE: " + to_string(score) + "  TIME: " + to_string((int)ramainingtime), 0, 0, TJ_LEFT, Nexus::FONT_SMALL);

	// if gameover, display some text
	if (gameOver)
	{
		Nexus::DRAW_TXT("GAME OVER", centerX, centerY-50, TJ_CENTRE, Nexus::FONT_LARGE);
		Nexus::DRAW_TXT("'ESC' - Return to menu.", centerX, centerY, TJ_CENTRE, Nexus::FONT_SMALL);
	}
}

/* Clean and unload assets for the PLAY game state */
void PlayState::CLEAN()
{
	Simulation::CLEAN();
}

/* Set game difficulty, intended for used by other states 

_difficulty - New difficulty
*/
void PlayState::SET_DIFFICULTY(int _difficulty)
{
	difficulty = _difficulty;
}