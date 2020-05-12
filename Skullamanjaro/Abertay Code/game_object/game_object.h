#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>

// Enum to denote entity type. Used for collision events.
enum ObjectType
{
	PLAYER,
	ENEMY,
	LEFT_WALL,
	RIGHT_WALL,
};

/* GameObject class, stores a mesh instance and objectType together */
class GameObject : public gef::MeshInstance
{
public:
	void UpdateFromSimulation(const b2Body* body);
	ObjectType type_;
	int collideCooldown = 0;
};

#endif // _GAME_OBJECT_H