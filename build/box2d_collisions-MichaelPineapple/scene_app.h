#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>

namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

/* SceneApp class, main entry point of the GEF platform application */
class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
};


#endif // _SCENE_APP_H
