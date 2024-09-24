#pragma once
#include "ECSIncludes.h"
#include "ComponentArray.h"

class ComponentManager
{
public:
	ComponentManager() {};
	~ComponentManager() 
	{
		for (auto& pair : componentArrays)
		{
			delete pair.second;
		}

		componentArrays.clear();
	};

	void Clear()
	{
		for (auto& pair : componentArrays)
		{
			pair.second->Clear();
		}
	}

	template <typename T>
	T* AddComponent(const Entity& entity)
	{
		unsigned int componentID = GetComponentID<T>();

		return GetComponentArray<T>(componentID)->Add(entity);
	}

	template <typename T>
	T* GetComponent(const Entity& entity)
	{
		unsigned int componentID = GetComponentID<T>();

		if (componentArrays.find(componentID) == componentArrays.end())
		{
			return nullptr;
		}

		return GetComponentArray<T>(componentID)->Get(entity);
	}

	template <typename T>
	void RemoveComponent(const Entity& entity)
	{
		unsigned int componentID = GetComponentID<T>();
		auto componentArray = GetComponentArray<T>(componentID);

		if (componentArray)
		{
			componentArray->Remove(entity);
		}
	}

	template <typename T>
	void CreateComponentArray()
	{
		unsigned int componentID = GetComponentID<T>();
		componentArrays.insert({ componentID, new ComponentArray<T>() });
	}

	IComponentArray* operator[](const unsigned int& i)
	{
		return componentArrays[i];
	}

	void Serialize(const Entity& entity, std::ofstream& ofs)
	{
		for (auto& component : componentArrays)
		{
			if (component.second->Serialize(entity, ofs))
			{
				ofs << "-";
			}
		}
	}

	std::unordered_map<unsigned int, IComponentArray*> GetArrays()
	{
		return componentArrays;
	}

private:
	template <typename T>
	ComponentArray<T>* GetComponentArray(unsigned int componentID)
	{
		auto it = componentArrays.find(componentID);

		if (it != componentArrays.end()) {
			return static_cast<ComponentArray<T>*>(it->second);
		}

		return nullptr;
	}

	std::unordered_map<unsigned int, IComponentArray*> componentArrays;
};