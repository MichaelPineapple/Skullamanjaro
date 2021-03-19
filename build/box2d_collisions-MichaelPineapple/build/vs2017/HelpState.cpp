#include <Nexus.h>

// screen size variables for positioning text
float centerX3, centerY3, height;

// movement speed of the player
const float playerMovementSpeed3 = 0.25f;

// the player's current left or right velocity
float playervel3 = 0.0f;

// variable to store the user's score
int score3 = 0;

// player object
MclObject* player3;

// starting position for the singular skull in this mode
const b2Vec2 skullStart(0.0f, 10.0f);

/* Modify the score

_p - Points to add
*/
void modPoints3(int _p)
{
	// if score is being increased, play the good sound, if being decreased play the bad sound
	if (_p > 0) Nexus::PLAY_AUDIO(Nexus::SOUND_GOOD);
	else Nexus::PLAY_AUDIO(Nexus::SOUND_BAD);

	// modify the score
	score3 += _p;
}

/* OnCollide callback, called when two Box2D bodies collide

_bodyA - One of the two bodies in collision
_bodyB - The other body in collision
*/
void onCollide3(b2Body* _bodyA, b2Body* _bodyB)
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

			if (typeA != typeB) // check the two objects are not the same type of entity
			{
				if (typeB == ENEMY) // check if object B is of type ENEMY
				{
					if (typeA == PLAYER)
					{
						// if object A is type PLAYER, bounce object B and increase score by 1
						_bodyB->ApplyForce(b2Vec2(playervel3, 40.0f), _bodyB->GetPosition(), true);
						modPoints3(1);
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

/* Initialize the HELP game state

_platform - Application platform
*/
void HelpState::INIT(Platform& _platform)
{
	// Initialize physics simulation
	Simulation::INITIALIZE(_platform, -1.0f);

	// get some data from the platform
	centerX3 = _platform.width() / 2.0f;
	centerY3 = _platform.height() / 2.0f;
	height = _platform.height();

	// Create left and right walls to keep skulls from moving off the screen
	MclObject* leftWall = Simulation::CREATE_ENTITY(b2Vec2(-7.6f, 0.0f), b2Vec2(1.0f, 200.0f), false, LEFT_WALL);
	MclObject* rightWall = Simulation::CREATE_ENTITY(b2Vec2(7.6f, 0.0f), b2Vec2(1.0f, 200.0f), false, RIGHT_WALL);
	leftWall->setVisible(false);
	rightWall->setVisible(false);
	Simulation::ADD_ENTITY(leftWall);
	Simulation::ADD_ENTITY(rightWall);

	// Create the player
	player3 = Simulation::CREATE_ENTITY(b2Vec2(0.0f, 0.5f), b2Vec2(2.5f, 0.8f), false, PLAYER, Nexus::MESH_PILLOW);
	Simulation::ADD_ENTITY(player3);

	// Add a single skull
	Simulation::ADD_ENTITY(skullStart, b2Vec2(0.4f, 0.5f), true, ENEMY, Nexus::MESH_SKULL);
}

/* Update the HELP game state

_kb - Keyboard input data
_platform - Application platform
_frameTime - Elapsed time on the last frame
*/
void HelpState::UPDATE(Keyboard* _kb, Platform& _platform, float _frameTime)
{
	// Update the phyics simulation, pass it the onCollide callback
	Simulation::UPDATE(onCollide3);

	// If a skull falls below the screen, decrease the score by 5 and reset the skull
	vector<MclObject*> entList = Simulation::GET_ENTITIES();
	for (int i = 0; i < entList.size(); i++)
	{
		MclObject* ent = entList[i];
		if (ent->getPos().y < -1.0f)
		{
			modPoints3(-5);
			ent->setPos(skullStart);
			ent->setVel(b2Vec2_zero);
		}
	}

	// Control the player. LEFT key moves the pillow left and RIGHT key moves it right.
	playervel3 = 0.0f;
	if (_kb->IsKeyDown(_kb->KC_LEFT))
	{
		if (player3->getPos().x > -6.0f) player3->move(b2Vec2(-playerMovementSpeed3, 0.0f));
		playervel3 = -10.0f;
	}
	else if (_kb->IsKeyDown(_kb->KC_RIGHT))
	{
		if (player3->getPos().x < 6.0f) player3->move(b2Vec2(playerMovementSpeed3, 0.0f));
		playervel3 = 10.0f;
	}

	// If user presses ESC, go to the MENU gamestate
	if (_kb->IsKeyPressed(_kb->KC_ESCAPE))
	{
		Nexus::CHANGE_STATE(MENU, _platform);
	}
}

/* Render 3D meshes for the HELP game state */
void HelpState::RENDER()
{
	Simulation::RENDER();
}

/* Render 2D bitmaps for the HELP game state */
void HelpState::RENDER_HUD()
{
	// display the score
	Nexus::DRAW_TXT("SCORE: " + to_string(score3), 0, 0, TJ_LEFT, Nexus::FONT_SMALL);

	// display some instructions
	Nexus::DRAW_TXT("Score points by bouncing skulls off the pillow before time runs out.", centerX3, centerY3-25, TJ_CENTRE, Nexus::FONT_SMALL); 
	Nexus::DRAW_TXT("You lose 5 points if a skull falls below the screen.", centerX3, centerY3, TJ_CENTRE, Nexus::FONT_SMALL);
	Nexus::DRAW_TXT("Move the pillow left and right using the arrow keys.", centerX3, centerY3+25, TJ_CENTRE, Nexus::FONT_SMALL);
	Nexus::DRAW_TXT("'ESC' - Return to menu.", 0, height-25, TJ_LEFT, Nexus::FONT_SMALL);
}

/* Clean and unload assets for the HELP game state */
void HelpState::CLEAN()
{
	Simulation::CLEAN();
}