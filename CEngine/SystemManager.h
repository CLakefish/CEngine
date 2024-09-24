#pragma once
#include "ECSIncludes.h"
#include "System.h"

class SystemManager
{
public:
	void RegisterSystem(System* system)
	{
		std::bitset<MAX_COMPONENTS> signature = system->GetSignature();

		if (systems.find(signature) != systems.end())
		{
			TraceMessage("System with signature ", signature, " already exists in SystemManager.");
			return;
		}

		systems.insert({ signature, system });

		if (system->ShouldContinuouslyUpdate())
		{
			updateSystems.push_back(system);
		}
	}

	void UpdateSystems()
	{
		for (auto& system : updateSystems)
		{
			system->Update();
		}
	}

	void AddToSystem(const EntityInfo& entityInfo)
	{
		for (auto& system : systems)
		{
			std::bitset<MAX_COMPONENTS> sig = system.second->GetSignature();
			if ((sig & entityInfo.mask) == sig)
			{
				system.second->Add(entityInfo.ID);
			}
		}
	}

	void RemoveFromSystem(const EntityInfo& entityInfo)
	{
		for (auto& system : systems)
		{
			auto& sig = system.second->GetSignature();
			if ((sig & entityInfo.mask) == sig)
			{
				system.second->Remove(entityInfo.ID);
			}
		}
	}

	void Clear() 
	{
		for (auto& system : systems)
		{
			delete system.second;
		}

		systems.clear();
		updateSystems.clear();
	}

	System* GetSystem(std::bitset<MAX_COMPONENTS> signature)
	{
		return systems[signature];
	}

private:
	std::vector<System*> updateSystems;
	std::unordered_map<std::bitset<MAX_COMPONENTS>, System*> systems;
};