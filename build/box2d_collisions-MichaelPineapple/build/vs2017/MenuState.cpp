#include "Nexus.h"

/* MenuOption struct, used to store all relevant data and functions for a menu options */
struct MenuOption
{
public:
	
	/* MenuOption constructor

	_txt - Text of the option
	_ypos - Y posititon of the option
	_enter - Function which is called when the user presses the ENTER key on that option
	_left - Function which is called when the user presses the LEFT key on that option
	_right - Function which is called when the user presses the RIGHT key on that option
	*/
	MenuOption(string _txt, float _ypos, void(*_enter)(Platform& _platform), void(*_left)(), void(*_right)()) :
		text(_txt), yPos(_ypos), enterFunc(_enter), leftFunc(_left), rightFunc(_right) {}

	// I think you get it
	string text;
	float yPos;
	void(*enterFunc)(Platform& _platform);
	void(*leftFunc)();
	void(*rightFunc)();
};

// variables to score different positons on the screen
float middleX, middleY, titleY;

// the current menu selection
int menuSelection = 0;

// sprite to show which menu option is selected
Sprite selectionSprite;

// settings variables
int diff = 0, vol = 100;

// list of menu options
vector<MenuOption*> menuOptionsList;

// menu options
MenuOption* start;
MenuOption* help;
MenuOption* difficulty;
MenuOption* volume;
MenuOption* quit;

// menu option functions to avoid redundant code
void playSound00() { Nexus::PLAY_AUDIO(Nexus::SOUND_MENU0); }
void playSound01() { Nexus::PLAY_AUDIO(Nexus::SOUND_MENU1); }
void updateVolume() { Nexus::SET_VOLUME(vol); }
void incDiff() { diff++; if (diff > 2) diff = 0; playSound01(); }
void decDiff() { diff--; if (diff < 0) diff = 2; playSound01(); }
void incVol() { vol++; if (vol > 100) vol = 0; updateVolume(); playSound01(); }
void decVol() { vol--; if (vol < 0) vol = 100; updateVolume(); playSound01(); }

/* Returns a random position above the screen, used for spawning new skulls

_y - Y value of the random position
*/
b2Vec2 getRndBallStart2(float _y)
{
	return b2Vec2(Nexus::RND(-10, 10), _y);
}

/* Initialize the MENU game state

_platform - Application platform
*/
void MenuState::INIT(Platform& _platform)
{
	// get text positioning data based on screen size
	middleX = (_platform.width() / 2.0f);
	middleY = (_platform.height() / 2.0f);
	titleY = middleY - 200;

	// create the 'start' menu option which allows the user to start the game
	start = new MenuOption("Start", middleY - 50, 
		[](Platform& _platform) { Nexus::PLAY_AUDIO(Nexus::SOUND_MENU2); PlayState::SET_DIFFICULTY(diff);  Nexus::CHANGE_STATE(PLAY, _platform); }, []() {}, []() {});

	// create the 'help' menu option which allows to user to access the HELP gamestate
	help = new MenuOption("How To Play", middleY - 0,
		[](Platform& _platform) { Nexus::CHANGE_STATE(HELP, _platform); }, []() {}, []() {});

	// create the 'difficulty' menu option which allows the user to change the difficulty
	difficulty = new MenuOption("Difficulty:", middleY + 50, 
		[](Platform& _platform) { incDiff();  }, decDiff, incDiff);

	// create the 'volume' menu options which allows the user to change the audio volume
	volume = new MenuOption("Volume:", middleY + 100, 
		[](Platform& _platform) { incVol(); }, decVol, incVol);

	// create the 'quit' menu option which allows the user to quit the game
	quit = new MenuOption("Quit", middleY + 150, 
		[](Platform& _platform) { exit(0); }, []() {}, []() {});

	// add menu option pointers to the menu options list
	menuOptionsList = vector<MenuOption*>();
	menuOptionsList.push_back(start);
	menuOptionsList.push_back(help);
	menuOptionsList.push_back(difficulty);
	menuOptionsList.push_back(volume);
	menuOptionsList.push_back(quit);

	// create the selection sprite
	Colour red(100, 0, 0);
	selectionSprite.set_width(250);
	selectionSprite.set_height(30);
	selectionSprite.set_colour(red.GetRGBA());
	selectionSprite.set_position(-1000, 0, 0);

	// initialize physics simulation
	Simulation::INITIALIZE(_platform, -1.0f);

	// generate 20 skulls above the screen for a cool background
	for (int i = 0; i < 20; i++)
	{
		MclObject* skulltmp = Simulation::CREATE_ENTITY(getRndBallStart2(10.0f + i * 5.0f), b2Vec2(0.4f, 0.5f), true, ENEMY, Nexus::MESH_SKULL);
		skulltmp->getBody()->SetAngularVelocity(Nexus::RND(-100, 100) / 10.0f);
		Simulation::ADD_ENTITY(skulltmp);
	}
}


/* Update the MENU game state

_kb - Keyboard input data
_platform - Application platform
_frameTime - Elapsed time on the last frame
*/
void MenuState::UPDATE(Keyboard* _kb, Platform& _platform, float _frameTime)
{
	// allow the user to move up and down the menu options with the UP and DOWN arrow keys, play appropriate sounds
	if (_kb->IsKeyPressed(_kb->KC_DOWN)) 
	{ 
		if (menuSelection < menuOptionsList.size()-1) menuSelection++; 
		playSound00();
	}
	else if (_kb->IsKeyPressed(_kb->KC_UP)) 
	{ 
		if (menuSelection > 0) menuSelection--; 
		playSound00();
	}

	// if user presses ENTER, LEFT, or RIGHT keys, call the current menu option's appropriate function
	else if (_kb->IsKeyPressed(_kb->KC_RETURN)) menuOptionsList[menuSelection]->enterFunc(_platform);
	else if (_kb->IsKeyPressed(_kb->KC_LEFT)) menuOptionsList[menuSelection]->leftFunc();
	else if (_kb->IsKeyPressed(_kb->KC_RIGHT)) menuOptionsList[menuSelection]->rightFunc();

	// depending on the current difficulty setting, display different text on the 'difficulty' menu option
	char* diffTxt = "Difficulty: NULL1";
	switch (diff)
	{
		case 0: diffTxt = "Difficulty: PEON"; break;
		case 1: diffTxt = "Difficulty: HERO"; break;
		case 2: diffTxt = "Difficulty: LEGEND"; break;
		default: diffTxt = "Difficulty: NULL2"; break;
	}
	difficulty->text = diffTxt;
	volume->text = "Volume: " + to_string(vol);

	// position the 'selectionSprite' at the current menu selection
	selectionSprite.set_position(Vector4(middleX, menuOptionsList[menuSelection]->yPos+15, 0));

	// update the physics simulation
	Simulation::UPDATE();

	// If a skull falls below the screen, reset the skull
	vector<MclObject*> entList = Simulation::GET_ENTITIES();
	for (int i = 0; i < entList.size(); i++)
	{
		MclObject* ent = entList[i];
		if (ent->getPos().y < -1.0f)
		{
			ent->setPos(getRndBallStart2(10.0f));
			ent->setVel(b2Vec2_zero);
		}
	}
}


/* Render 3D meshes for the MENU gamestate */
void MenuState::RENDER()
{
	Simulation::RENDER();
}


/* Render 2D bitmaps for the MENU gamestate */
void MenuState::RENDER_HUD()
{
	// draw the game title
	Nexus::DRAW_TXT("SKULLAMANJARO", middleX, titleY, TJ_CENTRE, Nexus::FONT_LARGE);

	// draw the selection sprite
	Nexus::DRAW_SPRITE(selectionSprite);

	// draw the menu options
	for (int i = 0; i < menuOptionsList.size(); i++)
	{
		MenuOption m = *menuOptionsList[i];
		Nexus::DRAW_TXT(m.text, middleX, m.yPos, TJ_CENTRE, Nexus::FONT_SMALL);
	}
}

/* Unload and clean the MENU gamestate */
void MenuState::CLEAN()
{
	Simulation::CLEAN();
}