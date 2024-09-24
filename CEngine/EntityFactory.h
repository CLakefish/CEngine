#pragma once
#include <unordered_set>
#include "ECSIncludes.h"

class EntityFactory
{
public:
	Entity Instantiate()
	{
		Entity newEntity;

		if (freeEntities.empty())
		{
			newEntity = nextEntity++;
		}
		else
		{
			newEntity = *freeEntities.begin();
			freeEntities.erase(newEntity);
		}

		entities[newEntity] = { newEntity, EntityMask() };
		return newEntity;
	}

	Entity InstantiateWithID(Entity id)
	{
		if (entities.find(id) != entities.end())
		{
			TraceMessage("Entity ID already exists in scene. ID provided: ", id, ". Creating new entity with differing ID");
			return Instantiate();
		}

		if (freeEntities.find(id) != freeEntities.end())
		{
			freeEntities.erase(id);
		}

		entities[id] = { id, EntityMask() };

		if (id >= nextEntity)
		{
			nextEntity = id + 1;
		}

		return id;
	}

	void Destroy(const Entity& entity)
	{
		auto iter = entities.find(entity);

		if (iter == entities.end())
		{
			TraceMessage("Unable to destroy entity of ID ", entity, ". Does not exist");
			return;
		}

		entities.erase(iter);
		freeEntities.insert(entity);
	}

	void Clear()
	{
		entities.clear();
		freeEntities.clear();
		nextEntity = 0;
	}

	void PrintAll() const
	{
		for (const auto& pair : entities)
		{
			std::cout << pair.second.ID << std::endl;
			std::cout << pair.second.mask << std::endl;
		}
	}

	Entity GetEntity(const unsigned int index)
	{
		return entities[index].ID;
	}

	EntityInfo& GetEntityInfo(const Entity& e)
	{
		return entities.at(e);
	}

	std::unordered_map<Entity, EntityInfo> GetAllEntityInfo() const
	{
		return entities;
	}

	EntityInfo& operator[](const Entity& entity)
	{
		return entities.at(entity);
	}

private:
	std::unordered_set<Entity>     freeEntities;
	std::unordered_map<Entity, EntityInfo> entities;
	Entity nextEntity = 0;
};