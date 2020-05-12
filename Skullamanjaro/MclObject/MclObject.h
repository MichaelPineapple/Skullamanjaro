#pragma once
#include <game_object.h>
#include <primitive_builder.h>
#include <box2d/Box2D.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh_instance.h>
#include <graphics/mesh.h>
#include <graphics/material.h>
using gef::Mesh;
using gef::Vector4;
using gef::Renderer3D;
using gef::MeshInstance;
using gef::Material;
using gef::Colour;

/* MclObject class, used to store a 'GameObject', and Box2D 'Body' in one single object. */
class MclObject
{
	public:

		// constructor
		MclObject(b2Vec2 _position, b2Vec2 _size, bool _dynamic, ObjectType _entityType, Mesh* _mesh, b2World* _world);

		// getters
		b2Vec2 getPos();
		b2Vec2 getVel();
		b2Body* getBody();
		bool isDynamic();
		ObjectType getEntityType();
		
		// setters
		void setVisible(bool _vis);
		void setPos(b2Vec2 _newPos);
		void setVel(b2Vec2 _newVel);
		void setColour(Colour _colour);
		void move(b2Vec2 _vec);
		void applyForce(b2Vec2 _force);

		// other
		void updateFromSimulation();
		void render(Renderer3D* _renderer);
		void clean();

	private:
		
		// private member variables
		Material material;
		GameObject gameObject;
		b2Body* body;
		bool visible = true;
		bool useColour = false;
};

