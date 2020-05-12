#pragma once

// Include and using statements.
#include <time.h>
#include <vector>
#include <string>
#include <box2d/Box2D.h>
#include <system/platform.h>
#include <system/debug_log.h>
#include <audio/audio_manager.h>
#include <graphics/sprite_renderer.h>
#include <graphics/renderer_3d.h>
#include <graphics/font.h>
#include <graphics/sprite.h>
#include <graphics/scene.h>
#include <graphics/texture.h>
#include <graphics/image_data.h>
#include <assets/png_loader.h>
#include <input/input_manager.h>
#include <input/keyboard.h>
#include <game_object.h>
#include <primitive_builder.h>
#include <MclObject.h>
#include <platform/d3d11/system/platform_d3d11.h>
using std::vector;
using std::string;
using std::to_string;
using gef::PNGLoader;
using gef::ImageData;
using gef::Texture;
using gef::AudioManager;
using gef::Scene;
using gef::Colour;
using gef::Sprite;
using gef::Renderer3D;
using gef::InputManager;
using gef::MeshInstance;
using gef::Platform;
using gef::Keyboard;
using gef::SpriteRenderer;
using gef::Font;
using gef::Renderer3D;
using gef::TextJustification;
using gef::DebugOut;
using gef::TJ_CENTRE;
using gef::TJ_RIGHT;
using gef::TJ_LEFT;

// Enum to denote game states.
enum State
{
	NONE,
	MENU,
	PLAY,
	INTRO,
	HELP,
};

/* Nexus class, central hub of the application. 
> Provides static global functions for loading & rendering assets.
> Stores some global pre-loaded assets for use throughout the game.
> Initialization, updating, rendering, cleaning, and switching game states are all done through this class.
*/
class Nexus
{
public:
	static void INITIALIZE(State _startState, Platform& _platform, Renderer3D* _renderer3d);
	static void UPDATE(Keyboard* _kb, Platform& _platform, float _frameTime);
	static void RENDER();
	static void CLEAN();
	static void CHANGE_STATE(State _newState, Platform& _platform);

	static int LOAD_AUDIO(string _filename, Platform& _platform);
	static void PLAY_AUDIO(int _audioID);
	static void PLAY_MUSIC();
	static void STOP_MUSIC();
	static void SET_VOLUME(int _vol);
	static int GET_IMPACT_SOUND();

	static Sprite LOAD_SPRITE(string _filename, Platform& _platform);
	static Mesh* LOAD_MESH(string _filename, Platform& _platform);

	static void DRAW_TXT(string _txt, float _x, float _y, TextJustification _tj, Font* _font);
	static void DRAW_SPRITE(Sprite _sprite);
	static void DRAW_MCLOBJ(MclObject* _mcl);

	static PrimitiveBuilder* GET_PRIMITIVE_BUILDER();

	static int RND(int _min, int _max);

	static Font* FONT_LARGE;
	static Font* FONT_SMALL;
	static Mesh* MESH_PILLOW;
	static Mesh* MESH_SKULL;
	static int SOUND_MENU0, SOUND_MENU1, SOUND_MENU2, SOUND_GOOD, SOUND_BAD;
};

/* Simulation class, used to manage the Box2d physics simulation.
> Provides functions to easily add entities to the simulation.
> Provides functions to intialize, update, render, and clean the simulation.
*/
class Simulation
{
public:
	static void INITIALIZE(Platform& _platform, float _gravity);
	static MclObject* CREATE_ENTITY(b2Vec2 _position, b2Vec2 _size, bool _dynamic, ObjectType _entityType);
	static MclObject* CREATE_ENTITY(b2Vec2 _position, b2Vec2 _size, bool _dynamic, ObjectType _entityType, Mesh* _mesh);
	static void ADD_ENTITY(MclObject* _obj);
	static void ADD_ENTITY(b2Vec2 _position, b2Vec2 _size, bool _dynamic, ObjectType _entityType);
	static void ADD_ENTITY(b2Vec2 _position, b2Vec2 _size, bool _dynamic, ObjectType _entityType, Mesh* _mesh);
	static void UPDATE(void (*_onCollide)(b2Body*, b2Body*));
	static void UPDATE();
	static void RENDER();
	static void CLEAN();
	static vector<MclObject*> GET_ENTITIES();
};

/* MenuState class, contains all functions related to the MENU gamestate */
class MenuState
{
public:
	static void INIT(Platform& _platform);
	static void UPDATE(Keyboard* _kb, Platform& _platform, float _frameTime);
	static void RENDER();
	static void RENDER_HUD();
	static void CLEAN();
};

/* IntroState class, contains all functions related to the INTRO gamestate */
class IntroState
{
public:
	static void INIT(Platform& _platform);
	static void UPDATE(Keyboard* _kb, Platform& _platform, float _frameTime);
	static void RENDER();
	static void RENDER_HUD();
	static void CLEAN();
};

/* PlayState class, contains all functions related to the PLAY gamestate */
class PlayState
{
public:
	static void INIT(Platform& _platform);
	static void UPDATE(Keyboard* _kb, Platform& _platform, float _frameTime);
	static void RENDER();
	static void RENDER_HUD();
	static void CLEAN();
	static void SET_DIFFICULTY(int _difficulty);
};

/* HelpState class, contains all functions related to the HELP gamestate */
class HelpState
{
public:
	static void INIT(Platform& _platform);
	static void UPDATE(Keyboard* _kb, Platform& _platform, float _frameTime);
	static void RENDER();
	static void RENDER_HUD();
	static void CLEAN();
};