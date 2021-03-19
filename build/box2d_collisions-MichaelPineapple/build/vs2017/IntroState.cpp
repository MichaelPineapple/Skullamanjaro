#include "Nexus.h"

// keep track of how much time has passed.
float timer = 0.0f;

// The sprite for this "Splash Screen"
Sprite splash;

/* Initialize the INTRO game state

_platform - Application platform
*/
void IntroState::INIT(Platform& _platform)
{
	// Load the splash screen image
	splash = Nexus::LOAD_SPRITE("wizlon_logo.png", _platform);
	splash.set_position(Vector4(_platform.width()/2.0f, _platform.height()/2.0f, 0));
}

/* Update the INTRO game state

_kb - Keyboard input data
_platform - Application platform
_frameTime - Elapsed time on the last frame
*/
void IntroState::UPDATE(Keyboard* _kb, Platform& _platform, float _frameTime)
{
	// When 1.5 seconds have elapsed, go to the MENU game state
	timer += _frameTime;
	if (timer > 1.5f)
	{
		Nexus::CHANGE_STATE(MENU, _platform);
		timer = 0.0f;
	}
}

/* Render 3D meshes for the INTRO game state (Do nothing) */
void IntroState::RENDER(){}

/* Render 2D bitmaps for the INTRO game state */
void IntroState::RENDER_HUD()
{
	// Draw the spash screen image
	Nexus::DRAW_SPRITE(splash);
}

/* Unload and clean assets for the INTRO game state (Do nothing) */
void IntroState::CLEAN(){}