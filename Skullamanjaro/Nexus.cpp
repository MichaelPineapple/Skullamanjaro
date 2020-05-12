#include "Nexus.h"

// Manager classes used to interface with the gef framework
AudioManager* audioManager;
SpriteRenderer* spriteRenderer;
Renderer3D* renderer3d;
PrimitiveBuilder* primitiveBuilder;

// variable to track the current game state
State currentGameState = NONE;

// variable to track if music is currently playing
bool musicPlaying = false;

// global font objects
Font* Nexus::FONT_LARGE;
Font* Nexus::FONT_SMALL;

// global mesh objects
Mesh* Nexus::MESH_PILLOW;
Mesh* Nexus::MESH_SKULL;

// global audio objects
int Nexus::SOUND_MENU0;
int Nexus::SOUND_MENU1;
int Nexus::SOUND_MENU2;
int Nexus::SOUND_GOOD;
int Nexus::SOUND_BAD;
const int impactSoundsArraySize = 3;
int impactSounds[3];

/* Loads 'Scene' object from a file

_filename - Location of the .scn file.
_platform - Current application platform.
*/
Scene* loadSceneAssets(Platform& _platform, const char* _filename)
{
	Scene* scene = new Scene();

	if (scene->ReadSceneFromFile(_platform, _filename))
	{
		scene->CreateMaterials(_platform);
		scene->CreateMeshes(_platform);
	}
	else
	{
		delete scene;
		scene = NULL;
	}

	return scene;
}

/* Loads 'Mesh' object from a 'Scene' object

_scene - 'Scene' object from which to extract the mesh
*/
Mesh* getMeshFromSceneAssets(Scene* _scene)
{
	Mesh* mesh = NULL;
	if (_scene && _scene->meshes.size() > 0) mesh = _scene->meshes.front();
	return mesh;
}

/* Loads 'Mesh' object from a file 

_filename - Location of the mesh .scn file.
_platform - Current application platform.
*/
Mesh* Nexus::LOAD_MESH(string _filename, Platform& _platform)
{
	return getMeshFromSceneAssets(loadSceneAssets(_platform, _filename.c_str()));
}

/* Loads audio asset from a file, stores the audio in 'audioManager' and returns its sample ID

_filename - Location of the audio file.
_platform - Current application platform.
*/
int Nexus::LOAD_AUDIO(string _filename, Platform& _platform)
{
	return audioManager->LoadSample(("audio/" + _filename).c_str(), _platform);
}

/* Loads 'Sprite' object from a file

_filename - Location of the sprite image file.
_platform - Current application platform.
*/
Sprite Nexus::LOAD_SPRITE(string _filename, Platform& _platform)
{
	ImageData imageData;
	PNGLoader pngLoader;
	pngLoader.Load(_filename.c_str(), _platform, imageData);
	Sprite $return;
	$return.set_texture(Texture::Create(_platform, imageData));
	$return.set_width(imageData.width());
	$return.set_height(imageData.height());
	$return.set_position(Vector4(0.0f, 0.0f, 0.0f));
	return $return;
}

/* Initialize the 'Nexus' class

_startState - State to start the game in, should be 'INTRO' usually
_platform - The current application platform
_renderer3d - Object used to render 3d meshes
*/
void Nexus::INITIALIZE(State _startState, Platform& _platform, Renderer3D* _renderer3d)
{
	srand(time(NULL));

	// initialze the manager objects which interface with the GEF framework
	audioManager = AudioManager::Create();
	audioManager->LoadMusic("audio/music2.ogg", _platform);
	spriteRenderer = SpriteRenderer::Create(_platform);
	renderer3d = _renderer3d;
	primitiveBuilder = new PrimitiveBuilder(_platform);

	// loads global fonts
	FONT_LARGE = new Font(_platform);
	FONT_SMALL = new Font(_platform);
	FONT_LARGE->Load("fonts/font");
	FONT_SMALL->Load("fonts/font_small");

	// load global meshes
	MESH_SKULL = LOAD_MESH("skull.scn", _platform);
	MESH_PILLOW = LOAD_MESH("pillow.scn", _platform);

	// check meshes have loaded properly
	if (!MESH_SKULL || !MESH_PILLOW)
	{
		DebugOut("Mesh file failed to load!");

		MESH_SKULL = primitiveBuilder->CreateBoxMesh(Vector4(0.2f, 0.25f, 0.25f));
		MESH_PILLOW = primitiveBuilder->CreateBoxMesh(Vector4(1.25f, 0.4f, 0.25f));
	}

	// load audio
	SOUND_MENU0 = LOAD_AUDIO("select_008.ogg", _platform);
	SOUND_MENU1 = LOAD_AUDIO("switch_007.ogg", _platform);
	SOUND_MENU2 = LOAD_AUDIO("select_006.ogg", _platform);
	SOUND_GOOD = LOAD_AUDIO("point.ogg", _platform);
	SOUND_BAD = LOAD_AUDIO("fail.ogg", _platform);
	impactSounds[0] = LOAD_AUDIO("impact07.ogg", _platform);
	impactSounds[1] = LOAD_AUDIO("impact08.ogg", _platform);
	impactSounds[2] = LOAD_AUDIO("impact09.ogg", _platform);

	// set the current state to the provided startstate
	CHANGE_STATE(_startState, _platform);
}

/* Returns a random pre-loaded impact sound */
int Nexus::GET_IMPACT_SOUND()
{
	return impactSounds[Nexus::RND(0, impactSoundsArraySize - 1)];
}

/* Plays the provided audio

_audioID - ID of the sample to play
*/
void Nexus::PLAY_AUDIO(int _audioID)
{
	audioManager->PlaySample(_audioID, false);
}

/* Play the pre-loaded music */
void Nexus::PLAY_MUSIC()
{
	if (!musicPlaying)
	{
		audioManager->PlayMusic();
		musicPlaying = true;
	}
}

/* Stop the music if it is currently playing */
void Nexus::STOP_MUSIC()
{
	if (musicPlaying)
	{
		audioManager->StopMusic();
		musicPlaying = false;
	}
}

/* Set master volume level

_vol - The new volume (0-100)
*/
void Nexus::SET_VOLUME(int _vol)
{
	if (_vol > 100) _vol = 100;
	else if (_vol < 0) _vol = 0;
	audioManager->SetMasterVolume(_vol);
}

/* Draw text to the screen 

_txt - String of characters to draw.
_x - X position of the text.
_y - Y position of the text.
_tj - Justification of the text (left, right, center)
_font - Font to render the text with.
*/
void Nexus::DRAW_TXT(string _txt, float _x, float _y, TextJustification _tj, Font* _font)
{
	_font->RenderText(spriteRenderer, Vector4(_x, _y, 0.0f), 1.0f, 0xffffffff, _tj, _txt.c_str());
}


/* Draw sprite to the screen

_sprite - Sprite to draw.
*/
void Nexus::DRAW_SPRITE(Sprite _sprite)
{
	spriteRenderer->DrawSprite(_sprite);
}

/* Draw the 3d mesh stored within a MclObject 

_mcl - 'MclObject' to draw.
*/
void Nexus::DRAW_MCLOBJ(MclObject* _mcl)
{
	_mcl->render(renderer3d);
}

/* Returns the global primative builder */
PrimitiveBuilder* Nexus::GET_PRIMITIVE_BUILDER()
{
	return primitiveBuilder;
}

/* Returns a random integer between _min and _max

_min - The lower bound.
_max - The upper bounc.
*/
int Nexus::RND(int _min, int _max)
{
	return _min + (rand() % static_cast<int>(_max - _min + 1));
}

/* Update the current gamestate

_kb - Keyboard input data
_platform - Current application data
_frameTime - How many seconds the last frame took to calculate
*/
void Nexus::UPDATE(Keyboard* _kb, Platform& _platform, float _frameTime)
{
	switch (currentGameState)
	{
		case MENU:MenuState::UPDATE(_kb, _platform, _frameTime);break;
		case PLAY:PlayState::UPDATE(_kb, _platform, _frameTime);break;
		case INTRO:IntroState::UPDATE(_kb, _platform, _frameTime);break;
		case HELP:HelpState::UPDATE(_kb, _platform, _frameTime);break;
		default:break;
	}
}

/* Render the current gamestate */
void Nexus::RENDER()
{
	// Render 3D meshes
	renderer3d->Begin();
	switch (currentGameState)
	{
		case MENU:MenuState::RENDER();break;
		case PLAY:PlayState::RENDER();break;
		case INTRO:IntroState::RENDER();break;
		case HELP:HelpState::RENDER();break;
		default:break;
	}
	renderer3d->End();

	// Render 2d bitmaps like text and sprites
	spriteRenderer->Begin(false);
	if (FONT_LARGE && FONT_SMALL) // make sure all fonts are loaded properly
	{
		switch (currentGameState)
		{
			case MENU:MenuState::RENDER_HUD();break;
			case PLAY:PlayState::RENDER_HUD();break;
			case INTRO:IntroState::RENDER_HUD();break;
			case HELP:HelpState::RENDER_HUD();break;
			default:break;
		}
	}
	spriteRenderer->End();
}

/* Clean the current game state */
void cleanStates()
{
	switch (currentGameState)
	{
		case MENU:MenuState::CLEAN();break;
		case PLAY:PlayState::CLEAN();break;
		case INTRO:IntroState::CLEAN();break;
		case HELP:HelpState::CLEAN();break;
		default:break;
	}
}

/* Unload everything in the Nexus, called when the application is closing */
void Nexus::CLEAN()
{
	cleanStates();

	STOP_MUSIC();
	audioManager->UnloadMusic();
	audioManager->UnloadAllSamples();
	audioManager = NULL;

	delete spriteRenderer;
	spriteRenderer = NULL;

	delete primitiveBuilder;
	primitiveBuilder = NULL;

	delete FONT_LARGE;
	FONT_LARGE = NULL;

	delete FONT_SMALL;
	FONT_SMALL = NULL;
}

/* Change the current gamestate 

_newState - The state to change to.
_platform - Current application platform
*/
void Nexus::CHANGE_STATE(State _newState, Platform& _platform)
{
	// Clean the old gamestate
	cleanStates();

	// store the new gamestate
	currentGameState = _newState;

	// Initialize the new gamestate
	switch (currentGameState)
	{
		case MENU: MenuState::INIT(_platform); break;
		case PLAY: PlayState::INIT(_platform); break;
		case INTRO: IntroState::INIT(_platform); break;
		case HELP: HelpState::INIT(_platform); break;
		default: break;
	}
}

