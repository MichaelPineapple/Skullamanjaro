#include "Nexus.h"
#include "scene_app.h"
#include <maths/math_utils.h>
using gef::Vector4;
using gef::Default3DShaderData;
using gef::Matrix44;
using gef::Colour;
using gef::PointLight;
using gef::DegToRad;

// Object to get input data from GEF framework
InputManager* inputManager;

// frames per second
float fps = 0;

/* Setup lighting for the 3D scene

_renderer3D - Object used to render 3d meshes
*/
void SetupLights(Renderer3D* _renderer3d)
{
	Default3DShaderData& default_shader_data = _renderer3d->default_shader_data();
	default_shader_data.set_ambient_light_colour(Colour(0.25f, 0.25f, 0.25f, 1.0f));
	PointLight default_point_light;
	default_point_light.set_colour(Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}

/* Setup the camera for the 3D scene

_platform - Application platform
_renderer3D - Object used to render 3d meshes
*/
void SetupCamera(Platform& _platform, Renderer3D* _renderer3d)
{
	float fov = DegToRad(45.0f);
	float aspect_ratio = (float)_platform.width() / (float)_platform.height();
	Matrix44 projection_matrix;
	projection_matrix = _platform.PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	_renderer3d->set_projection_matrix(projection_matrix);
	Vector4 camera_eye(0.0f, 3.5f, 10.0f);
	Vector4 camera_lookat(0.0f, 3.5f, 0.0f);
	Vector4 camera_up(0.0f, 1.0f, 0.0f);
	Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	_renderer3d->set_view_matrix(view_matrix);
}

/* GEF application constructor */
SceneApp::SceneApp(Platform& platform) : Application(platform) { }


/* Initialize the GEF platform application */
void SceneApp::Init()
{
	// initialize the 3D renderer and input manager
	Renderer3D* renderer3d = Renderer3D::Create(platform_);
	inputManager = InputManager::Create(platform_);

	// initialize the camera
	SetupCamera(platform_, renderer3d);

	// setup lighting
	SetupLights(renderer3d);

	// Initialize the game
	Nexus::INITIALIZE(INTRO, platform_, renderer3d);
}

/* Update the GEF platform application

_frameTime - Elapsed time of the previous frame
*/
bool SceneApp::Update(float _frameTime)
{
	// calculate frames per second
	fps = 1.0f / _frameTime;

	// update the input manager
	inputManager->Update();

	// update the game 
	Nexus::UPDATE(inputManager->keyboard(), platform_, _frameTime);

	return true;
}

void SceneApp::Render()
{
	// render the game
	Nexus::RENDER();
}

void SceneApp::CleanUp()
{
	// clean up the game
	Nexus::CLEAN();

	// unload the input manager
	delete inputManager;
	inputManager = NULL;
}















//// LEGACY 


/*void SceneApp::InitPlayer()
{
	Vector4 player_half_dimensions(1.0f, 0.1f, 0.5f);

	// setup the mesh for the player
	player_.set_mesh(primitiveBuilder->CreateBoxMesh(player_half_dimensions));

	// create a physics body for the player
	b2BodyDef player_body_def;
	player_body_def.type = b2_staticBody;
	player_body_def.position = b2Vec2(0.0f, 0.0f);

	player_body_ = world->CreateBody(&player_body_def);

	// create the shape for the player
	b2PolygonShape player_shape;
	player_shape.SetAsBox(player_half_dimensions.x(), player_half_dimensions.y());

	// create the fixture
	b2FixtureDef player_fixture_def;
	player_fixture_def.shape = &player_shape;
	player_fixture_def.density = 1.0f;

	// create the fixture on the rigid body
	player_body_->CreateFixture(&player_fixture_def);

	// update visuals from simulation data
	player_.UpdateFromSimulation(player_body_);

	player_.type_ = PLAYER;
	player_body_->SetUserData(&player_);
}

void SceneApp::InitEnemy()
{
	float radius = 0.2f;

	// setup the mesh for the player
	enemy_.set_mesh(primitiveBuilder->CreateSphereMesh(radius, 20, 20, Vector4(0.0f, 0.0f, 0.0f), NULL));

	// create a physics body for the player
	b2BodyDef enemy_body_def;
	enemy_body_def.type = b2_dynamicBody;
	enemy_body_def.position = b2Vec2(0.0f, 10.0f);

	enemy_body_ = world->CreateBody(&enemy_body_def);

	// create the shape for the player
	b2PolygonShape enemy_shape;
	enemy_shape.SetAsBox(radius, radius);

	// create the fixture
	b2FixtureDef enemy_fixture_def;
	enemy_fixture_def.shape = &enemy_shape;
	enemy_fixture_def.density = 1.0f;

	// create the fixture on the rigid body
	enemy_body_->CreateFixture(&enemy_fixture_def);

	// update visuals from simulation data
	enemy_.UpdateFromSimulation(enemy_body_);

	enemy_.type_ = ENEMY;
	enemy_body_->SetUserData(&enemy_);
}


void SceneApp::InitWalls()
{
	// ground dimensions
	Vector4 wall_half_dimensions(0.1f, 10.0f, 0.5f);

	// setup the mesh for the ground
	wall_mesh_ = primitiveBuilder->CreateBoxMesh(wall_half_dimensions);
	left_wall_.set_mesh(wall_mesh_);
	right_wall_.set_mesh(wall_mesh_);

	// create a physics body
	b2BodyDef left_body_def;
	left_body_def.type = b2_staticBody;
	left_body_def.position = b2Vec2(-7.0f, 0.0f);
	b2BodyDef right_body_def;
	right_body_def.type = b2_staticBody;
	right_body_def.position = b2Vec2(7.0f, 0.0f);

	left_wall_body_ = world->CreateBody(&left_body_def);
	right_wall_body_ = world->CreateBody(&right_body_def);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(wall_half_dimensions.x(), wall_half_dimensions.y());

	// create the fixture
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;

	// create the fixture on the rigid body
	left_wall_body_->CreateFixture(&fixture_def);
	right_wall_body_->CreateFixture(&fixture_def);

	// update visuals from simulation data
	left_wall_.UpdateFromSimulation(left_wall_body_);
	right_wall_.UpdateFromSimulation(right_wall_body_);
}
*/