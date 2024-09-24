#pragma once
#include "ECSIncludes.h"
#include <functional>

class Behavior
{
public:
	Behavior() {};
	~Behavior() {};

	virtual void Enter() {};
	virtual void Update() {};
	virtual void FixedUpdate() {};
	virtual void Exit() {};

	virtual void Reload() {};
	virtual void Serialize(std::ostream& os) {};
	virtual void DeSerialize(std::istream& is) {};
};

class BehaviorManager
{
public:
	BehaviorManager() {}

	~BehaviorManager()
	{
		Clear();
	}

	// Add type ensurance via dynamic cast for behavior
	template <typename T>
	T* AddBehavior(const Entity& entity)
	{
		auto entityIter	 = behaviorMap.find(entity);
		const char* typeName = typeid(T).name();
		T* behavior			 = new T();

		// If entity does not exist in vector
		if (entityIter == behaviorMap.end())
		{
			behaviorMap.insert({ entity, { std::vector<Behavior*>{behavior} } });
		}
		// If entity does exist in vector check each type ID to ensure that this component is wholely unique
		else
		{
			auto& entityBehaviorVector = entityIter->second;

			for (auto& behavior : entityBehaviorVector)
			{
				if (typeid(behavior).name() == typeName)
				{
					TraceMessage("Behavior of type ", typeName, " exists on entity of ID ", entity);
					delete behavior;
					return nullptr;
				}
			}

			entityIter->second.push_back(behavior);
		}

		// Insert into all map
		auto allMapIter = behaviorAllMap.find(typeName);

		if (allMapIter != behaviorAllMap.end())
		{
			allMapIter->second.push_back(behavior);
		}
		else
		{
			behaviorAllMap.insert({ typeName, std::vector<Behavior*>{behavior} });
		}

		behavior->Enter();
		return behavior;
	}

	template <typename T>
	T* GetBehavior(const Entity& entity)
	{
		T* behavior;

		for (auto& potentialBehavior : behaviorMap[entity])
		{
			behavior = dynamic_cast<T*>(potentialBehavior);
			if (behavior != nullptr) return behavior;
		}

		return nullptr;
	}
	
	template <typename T>
	T* FindBehavior()
	{
		const char* typeName = typeid(T).name();
		auto allMapIter = behaviorAllMap.find(typeName);

		if (allMapIter == behaviorAllMap.end())
		{
			TraceMessage("Unable to find component of type ", typeName);
			return nullptr;
		}

		Behavior* behavior = allMapIter->second[0];
		return dynamic_cast<T*>(behavior);
	}

	template <typename T>
	void RemoveBehavior(const Entity& entity)
	{
		T* behavior = nullptr;

		for (auto iter = behaviorMap[entity].begin(); iter < behaviorMap[entity].end(); ++iter)
		{
			behavior = dynamic_cast<T*>(*iter);

			if (behavior != nullptr)
			{
				behavior->Exit();
				
				std::vector<Behavior*>& behaviorVec = behaviorAllMap[typeid(T).name()];
				auto allIter = std::find(behaviorVec.begin(), behaviorVec.end(), behavior);
				if (allIter != behaviorVec.end())
				{
					behaviorVec.erase(allIter);
				}

				iter = behaviorMap[entity].erase(iter);
				delete behavior;

				return;
			}
		}

		TraceMessage("Behavior of type ", typeid(T).name(), " does not exist on entity with ID ", entity, ". It cannot be deleted if it doesnt exist 4head.");
	}

	std::vector<Behavior*> GetBehaviors(const Entity& entity)
	{
		return behaviorMap[entity];
	}

	void Clear()
	{
		for (auto& entity : behaviorMap)
		{
			for (auto& behavior : entity.second)
			{
				behavior->Exit();
				delete behavior;
			}

			entity.second.clear();
		}

		behaviorMap.clear();
	}

	template <typename T>
	void RegisterBehavior()
	{
		deserializationMap.insert
		({
			1,

			[=](const Entity& entity, std::istream& is)
			{
				auto behavior = AddBehavior<T>(entity);

				if (behavior != nullptr)
				{
					behavior->DeSerialize(is);
				}
			}
		});
	}

	void Update()
	{
		for (auto& mapIndex : behaviorAllMap)
		{
			std::vector<Behavior*> behaviorVec = mapIndex.second;

			for (auto& behavior : behaviorVec)
			{
				behavior->Update();
			}
		}
	}

	void FixedUpdate()
	{
		for (auto& mapIndex : behaviorAllMap)
		{
			std::vector<Behavior*> behaviorVec = mapIndex.second;

			for (auto& behavior : behaviorVec)
			{
				behavior->FixedUpdate();
			}
		}
	}

	std::unordered_map<Entity, std::vector<Behavior*>> GetMap()
	{
		return behaviorMap;
	}

private:
	std::unordered_map<unsigned int, std::function<void(const Entity&, std::istream&)>> deserializationMap;
	std::unordered_map<Entity, std::vector<Behavior*>> behaviorMap;
	std::unordered_map<const char*, std::vector<Behavior*>> behaviorAllMap;
};