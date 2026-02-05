#include"EntityManager.h"
#include<algorithm>

EntityManager::EntityManager()
	:m_totalEntities(0){ }

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	vec.erase(std::remove_if(vec.begin(), vec.end(),
		[](const std::shared_ptr<Entity>& e) { return !e->isActive(); }),
		vec.end());
}

void EntityManager::update()
{
    // 1. Add entities created in the last frame to the main lists
    for (auto& e : m_entitiesToAdd)
    {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e); // This requires EntityMap to be map<string, EntityVec>
    }
    m_entitiesToAdd.clear();

    // 2. Clean up dead entities using the erase-remove idiom you just learned
    removeDeadEntities(m_entities);

    // 3. Clean up dead entities from each vector inside the map
    for (auto& [tag, entityVec] : m_entityMap)
    {
        removeDeadEntities(entityVec);
    }
}


std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
    auto e = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

    m_entitiesToAdd.push_back(e);

    return e;
}

const EntityVec& EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
    return m_entityMap[tag];
}