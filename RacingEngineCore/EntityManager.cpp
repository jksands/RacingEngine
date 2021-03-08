#include "EntityManager.h"
EntityManager* EntityManager::instance = nullptr;

EntityManager* EntityManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new EntityManager();
    }

    return instance;
}

EntityManager::~EntityManager()
{
    for (auto rb : rigidbodies)
    {
        delete rb;
    }
    for (auto e : entities)
    {
        delete e;
    }
}

/// <summary>
/// Adds entitites to entity list
/// Adds rigidbody to rigidbody list if is physics object
/// </summary>
/// <param name="e"></param>
void EntityManager::AddEntity(Entity* e)
{
    entities.push_back(e);
    if (e->IsPhysicsObject())
    {
        rigidbodies.push_back(e->GetRigidBody());
    }
}

std::vector<Entity*> EntityManager::GetEntities()
{
	return entities;
}

std::vector<Rigidbody*> EntityManager::GetRigidBodies()
{
	return rigidbodies;
}
