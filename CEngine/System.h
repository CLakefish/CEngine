#pragma once
#include <bitset>
#include <vector>

#include "ECSIncludes.h"
#include "ComponentManager.h"

class System
{
public:
	System(ComponentManager& c) : componentManager(c) { };
	System(const std::bitset<MAX_COMPONENTS>& set, ComponentManager& c) : signature(set), componentManager(c) { };

	virtual ~System() = default;

	void SetSignature(const std::bitset<MAX_COMPONENTS>& set)
	{
		signature = set;
	}

	std::bitset<MAX_COMPONENTS>& GetSignature()
	{
		return signature;
	}

	void Add(const Entity& e)
	{
		OnAdd(e);
		entities.push_back(e);
	}

	void Remove(const Entity& e)
	{
		auto iter = std::find(entities.begin(), entities.end(), e);

		if (iter == entities.end()) 
		{
			TraceMessage("Entity with ID ", e, " not found in system.");
			return;
		}

		OnRemove(e);

		entities.erase(iter);
	}

	template <typename T>
	T* GetComponent(const Entity& entity)
	{
		return componentManager.GetComponent<T>(entity);
	}

	virtual void Update() {};
	virtual void OnAdd(const Entity& e)    {};
	virtual void OnRemove(const Entity& e) {};

	inline bool ShouldContinuouslyUpdate()
	{
		return continualUpdate;
	}

	inline void SetContinuousUpdate(const bool& val)
	{
		continualUpdate = val;
	}

protected:
	std::bitset<MAX_COMPONENTS> signature;
	std::vector<Entity> entities;
	ComponentManager& componentManager;

	bool continualUpdate = false;
};

class SystemTest : public System
{
public:
	SystemTest(ComponentManager& c) : System(c)
	{
		std::bitset<MAX_COMPONENTS> sig;
		sig.set(GetComponentID<Transform>());
		sig.set(GetComponentID<Mesh>());

		SetSignature(sig);

		SetContinuousUpdate(true);
	}

	void Update() override
	{
		std::cout << "Updating" << std::endl;

		for (auto& entity : entities)
		{
			Transform* t = GetComponent<Transform>(entity);
			std::cout << "Updating entity with ID: " << entity << std::endl;
			std::cout << t->x << std::endl;
			t->x++;
		}
	}

	void OnAdd(const Entity& e) override
	{
		std::cout << "Adding entity with ID: " << e << std::endl;
	}

	void OnRemove(const Entity& e) override
	{
		std::cout << "Removing entity with ID: " << e << std::endl;
	}
};