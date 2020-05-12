#include "MclObject.h"

/* MclObject constructor

_position - Starting position of the new MclObject
_size - Size of the new MclObject
_dynamic - If the MclObject should be affected by physics or not
_entityType - Type of entity this MclObject will represent
_mesh - Mesh of the MclObject
_world - World to add this MclObject to
*/
MclObject::MclObject(b2Vec2 _position, b2Vec2 _size, bool _dynamic, ObjectType _entityType, Mesh* _mesh, b2World* _world)
{
	// Set the mesh
	gameObject.set_mesh(_mesh);

	// Set the entity type
	gameObject.type_ = _entityType;

	// create the Box2D body
	b2BodyDef body_def;
	if (_dynamic) body_def.type = b2_dynamicBody;
	else body_def.type = b2_staticBody;
	body_def.position = _position;
	b2PolygonShape shape;
	shape.SetAsBox(_size.x/2.0f, _size.y/2.0f);
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;
	fixture_def.density = 1.0f;
	body = _world->CreateBody(&body_def);
	body->SetUserData(&gameObject);
	body->CreateFixture(&fixture_def);

	// prime body for physics
	gameObject.UpdateFromSimulation(body);

	// NO SLEEPING ALLOWED!
	body->SetSleepingAllowed(false);
}

/* Return this MclObject's position */
b2Vec2 MclObject::getPos()
{
	return this->body->GetPosition();
}

/* Return this MclObject's linear velocity */
b2Vec2 MclObject::getVel()
{
	return this->body->GetLinearVelocity();
}

/* Return this MclObject's Box2D body */
b2Body* MclObject::getBody()
{
	return body;
}

/* Return if this MclObject is dynamic */
bool MclObject::isDynamic()
{
	return (this->body->GetType() == b2_dynamicBody);
}

/* Return this MclObject's entity type */
ObjectType MclObject::getEntityType()
{
	return gameObject.type_;
}

/* Set if this MclObject should be visible

_visible - Bool representing the visiblilty (true = visible, false = invisible)
*/
void MclObject::setVisible(bool _visible)
{
	this->visible = _visible;
}

/* Set this MclObject's colour

_colour - The colour.
*/
void MclObject::setColour(Colour _colour)
{
	this->material = Material();
	this->material.set_colour(_colour.GetABGR());
	useColour = true;
}

/* Set this MclObject's position

_newPos - The new position
*/
void MclObject::setPos(b2Vec2 _newPos)
{
	body->SetTransform(_newPos, 0.0f);
}

/* Set this MclObject's linear velocity

_newVel - The new linear velocity
*/
void MclObject::setVel(b2Vec2 _newVel)
{
	body->SetLinearVelocity(_newVel);
}

/* Move this MclObject

_vec - Vector to be added to this MclObject's position
*/
void MclObject::move(b2Vec2 _vec)
{
	setPos(getPos() + _vec);
}

/* Apply a force to this MclObject

_force - "May the force be with you..."
*/
void MclObject::applyForce(b2Vec2 _force)
{
	body->ApplyForce(_force, getPos(), true);
}

/* Clean this MclObject */
void MclObject::clean()
{
	//delete gameObject.mesh;
	//gameObject.mesh = NULL;
}

/* Update this MclObject within its simulation */
void MclObject::updateFromSimulation()
{
	gameObject.UpdateFromSimulation(body);
}

/* Render this MclObject

_renderer - Object used to render 3D meshes
*/
void MclObject::render(Renderer3D* _renderer)
{
	if (visible)
	{
		if (useColour) _renderer->set_override_material(&material);
		else _renderer->set_override_material(NULL);
		_renderer->DrawMesh(gameObject);
	}
}