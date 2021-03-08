#pragma once
#include "Entity.h"
#include "Rigidbody.h"
#include <vector>
class EntityManager
{
private:
	static EntityManager* instance;
	std::vector<Entity*> entities;
	std::vector<Rigidbody*> rigidbodies;

public:
	/// <summary>
	/// Returns the singleton instance of the Entity Manager
	/// </summary>
	/// <returns>The singleton instance</returns>
	static EntityManager* GetInstance();
	~EntityManager();
	void AddEntity(Entity* e);
	std::vector<Entity*> GetEntities();
	std::vector<Rigidbody*> GetRigidBodies();

};

