#include "Nexus.h"

// List of entities (MclObjects) in the physics simulation
vector<MclObject*> entityList = vector<MclObject*>();

// THE WORLD!!
b2World* world;

/* Initialize the simulation

_platform - Current application platform.
_gravity - Gavity Y value for the Box2D physics simulation.
*/
void Simulation::INITIALIZE(Platform& _platform, float _gravity)
{
	world = new b2World(b2Vec2(0.0f, _gravity));
	entityList = vector<MclObject*>();
}

/* Return a new MclObject given some creation info

_position - Starting position of the MclObject
_size - Size of the MclObject
_dynamic - If the MclObject should be affected by the Box2D physics
_entityType - The type of entity this MclObject is.
_mesh - Mesh of the MclObject (Optional)
*/
MclObject* Simulation::CREATE_ENTITY(b2Vec2 _position, b2Vec2 _size, bool _dynamic, ObjectType _entityType)
{
	return new MclObject(_position, _size, _dynamic, _entityType, Nexus::GET_PRIMITIVE_BUILDER()->CreateBoxMesh(Vector4(_size.x/2.0f, _size.y/2.0f, 0.25f)), world);
}
MclObject* Simulation::CREATE_ENTITY(b2Vec2 _position, b2Vec2 _size, bool _dynamic, ObjectType _entityType, Mesh* _mesh)
{
	return new MclObject(_position, _size, _dynamic, _entityType, _mesh, world);
}

/* Add a new MclObject to the simulation

_obj - The MclObject to add.
*/
void Simulation::ADD_ENTITY(MclObject* _obj)
{
	entityList.push_back(_obj);
}

/* Create and add a new MclObject to the simulation given some creation info

_position - Starting position of the MclObject
_size - Size of the MclObject
_dynamic - If the MclObject should be affected by the Box2D physics
_entityType - The type of entity this MclObject is.
_mesh - Mesh of the MclObject (Optional)
*/
void Simulation::ADD_ENTITY(b2Vec2 _position, b2Vec2 _size, bool _dynamic, ObjectType _entityType)
{
	ADD_ENTITY(CREATE_ENTITY(_position, _size, _dynamic, _entityType));
}
void Simulation::ADD_ENTITY(b2Vec2 _position, b2Vec2 _size, bool _dynamic, ObjectType _entityType, Mesh* _mesh)
{
	ADD_ENTITY(CREATE_ENTITY(_position, _size, _dynamic, _entityType, _mesh));
}

/* Internal function to update the simulation */
void update()
{
	if (world)
	{
		// Update THE WORLD
		float timeStep = 1.0f / 60.0f;
		int32 velocityIterations = 6;
		int32 positionIterations = 2;
		world->Step(timeStep, velocityIterations, positionIterations);

		// Update all entities
		for (int i = 0; i < entityList.size(); i++)
		{
			entityList[i]->updateFromSimulation();

			// increment the collide cooldown for each entity (used for better collision handling)
			GameObject* obj = (GameObject*)entityList[i]->getBody()->GetUserData();
			obj->collideCooldown++;
		}
	}
}

/* Global function to update the simulation

_onCollide - Callback function when two Box2D bodies collide (Optional.)
*/
void Simulation::UPDATE(void (*_onCollide)(b2Body*, b2Body*))
{
	update();

	if (world)
	{
		b2Contact* contact = world->GetContactList();
		int contact_count = world->GetContactCount();
		for (int contact_num = 0; contact_num < contact_count; ++contact_num)
		{
			if (contact->IsTouching()) _onCollide(contact->GetFixtureA()->GetBody(), contact->GetFixtureB()->GetBody());
			contact = contact->GetNext();
		}
	}
}
void Simulation::UPDATE()
{
	update();
}

/* Render the simulation */
void Simulation::RENDER()
{
	for (int i = 0; i < entityList.size(); i++) Nexus::DRAW_MCLOBJ(entityList[i]);
}

/* Unload all entities and clean the simulation */
void Simulation::CLEAN()
{
	delete world;
	world = NULL;


	for (int i = 0; i < entityList.size(); i++) entityList[i]->clean();
	entityList.clear();
}

/* Return a reference to the list of all entities within the simulation */
vector<MclObject*> Simulation::GET_ENTITIES()
{
	return entityList;
}