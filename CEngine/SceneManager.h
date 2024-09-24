#pragma once
#include <vector>
#include <cassert>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <functional>

#include "ECSIncludes.h"
#include "ComponentArray.h"
#include "EntityFactory.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include "BehaviorTest.h"
#include "BehaviorManager.h"
#include "System.h"
#include <map>

struct Scene
{
	Scene(std::string _sceneName) : sceneName(_sceneName), scenePath(_sceneName + ".scene") {}

	std::string scenePath;
	std::string sceneName;
};

class SceneManager
{
public:
	SceneManager() { }

	~SceneManager()  {
		Clear();
	}

	// TO-DO : 
	// File corruption prevention (save 2nd copy, append each file w/byte count or something to determine if copies are different, can determine error saves from there)
	// 
	// System refactor
	// Messaging system
	// Collisions system refactor when I get there
	// 
	// Support behaviors attached to entities in serialization
	// Behavior serialization, should be done via an extra segment at the end of a scene file with entity indexes corresponding to either an int or a const char*
	//		from there I would need to serialize the script indexing, const char* to unsigned int, as to avoid issues when removing scripts or renaming scripts
	// 
	// Scene Manager
	// Changing between scenes
	// Cacheing?
	// Hot reloading?
	// 
	// BASELINE SHOULD BE SETUP
	// 
	// Setup ImGui visuals
	// Potential file explorer?
	// Hierarchy support
	// GLAD Rendering
	// Picking objects in scene
	// Moving/rotating/scaling objects in scene
	// Message system
	// 
	// Potential Optimizations down the road
	// - Change Scene Serialization so that it does not save entity IDs (by this I mean to remove the possibility of numbers being skipped in the scene, thus making serialization more optimized).
	//		- Would be somewhat unneccessary 
	// - Messaging system for system removal/addition/communication
	// 
	// Once everything else is semi-functional:
	// 1. A 3D editor, focusing on creating simple maps.
	//      - Pre-recs: Model Rendering, Model Loading, Grid Rendering, Prefabs, ImGui Inspector/Hierarchy
	//		- Setup should be very simplistic
	// 2. A game, imports the saved map from the 3D editor and then runs the code for movement
	//      - For this, add hot-reloading if possible. If not then try to create movement in unity then replicate in here.
	//      - Decide on a game idea to run with

	void Init()
	{
		// Initialize all components
		RegisterComponent<Transform>();
		RegisterComponent<Rigidbody>();
		RegisterComponent<Mesh>();
		RegisterComponent<BoxCollider>();
		RegisterComponent<SphereCollider>();
		RegisterComponent<CapsuleCollider>();
		RegisterComponent<MeshCollider>();
		RegisterComponent<ParticleSystem>();
		RegisterComponent<AudioSource>();
		RegisterComponent<LightSource>();

		RegisterSystem<SystemTest>();

		RegisterBehavior<BehaviorTest>();

		LoadScenes();
	}

	#pragma region Serialization

	void AddScene(Scene& scene)
	{
		for (auto const& s : scenes)
		{
			if (s.sceneName == scene.sceneName || s.scenePath == scene.scenePath)
			{
				TraceMessage("Scene with name and/or file path already exists! Name given: %s %s", scene.sceneName.c_str(), scene.scenePath.c_str());
				return;
			}
		}

		RegisterScene(scene);
		scenes.push_back(scene);
	}

	void AddScene(const char* name)
	{
		for (auto const& s : scenes)
		{
			if (s.sceneName == name)
			{
				TraceMessage("Scene with name already exists! Name given: %s", name);
				return;
			}
		}

		Scene scene = Scene(name);
		RegisterScene(scene);
		scenes.push_back(scene);
	}

	void RegisterScene(const Scene& scene)
	{
		std::ofstream os(sceneFile, std::ios::app);

		if (!os.is_open())
		{
			TraceMessage("Unable to load scene directory file in registry! Path given: %s", sceneFile);
			return;
		}

		os << scene.sceneName << '\n';
		os.close();
	}

	void LoadScenes()
	{
		std::ifstream is(sceneFile);

		if (!is.is_open())
		{
			TraceMessage("Unable to load scene directory file! Path given: %s", sceneFile);
			return;
		}

		if (is.peek() == std::ifstream::traits_type::eof())
		{
			AddScene("newScene");
			SaveScene();
			is.close();

			LoadScene(scenes[0]);
			return;
		}

		std::string line;
		while (std::getline(is, line))
		{
			if (line.empty()) continue;
			scenes.push_back(Scene(line));
		}

		is.close();

		if (scenes.empty())
		{
			TraceMessage("Unable to load in any scenes");
			return;
		}

		LoadScene(scenes[0]);
	}

	void LoadScene(const unsigned int& index)
	{
		TraceMessage("Loading scene with index %i", index);
		LoadScene(scenes[index]);
	}

	void LoadScene(const Scene& scene)
	{
		Clear();

		std::ifstream is(scene.scenePath, std::ios::binary);

		if (!is.is_open())
		{
			TraceMessage("Unable to load scene file! Path given: %s", scene.scenePath.c_str());
			return;
		}

		if (is.peek() == std::ifstream::traits_type::eof())
		{
			TraceMessage("Scene file is empty! Path given: %s", scene.scenePath.c_str());
			return;
		}

		TraceMessage("Loading scene %s", scene.scenePath.c_str());

		unsigned int entityCount = 0;
		READ_BINARY(is, entityCount);

		Entity entity;

		for (unsigned int i = 0; i < entityCount; ++i)
		{
			Entity entityID = 0;
			READ_BINARY(is, entityID);
			entity = entityFactory.InstantiateWithID(entityID);
			unsigned int componentCount = 0;
			READ_BINARY(is, componentCount);

			for (unsigned int j = 0; j < componentCount; ++j)
			{
				unsigned int componentID = 0;
				READ_BINARY(is, componentID);

				auto iter = componentReconstructMap.find(componentID);
				if (iter == componentReconstructMap.end())
				{
					TraceMessage("Entity ID %i with component ID %i not recognized.", entity, iter);
					continue;
				}

				iter->second(entity, is);
			}

			AddToSystem(entity);
		}

		unsigned int behaviorCount = 0;
		READ_BINARY(is, behaviorCount);

		for (unsigned int i = 0; i < behaviorCount; ++i)
		{
			unsigned int behaviorID = 0;
			READ_BINARY(is, behaviorID);
		}

		is.close();

		TraceMessage("Scene %s has been loaded successfully. Total entity count: %i", scene.scenePath.c_str(), entityCount);
	}

	void SaveScene()
	{
		Scene& scene = scenes[0];
		std::string filePath = scene.scenePath;
		std::ofstream os(filePath.c_str(), std::ios::binary);

		if (!os.is_open())
		{
			TraceMessage("Unable to load scene file! Path given: %s", scene.scenePath.c_str());
			return;
		}

		auto entityList = entityFactory.GetAllEntityInfo();
		auto componentArrays = componentManager.GetArrays();
		unsigned int entityCount = static_cast<unsigned int>(entityList.size());

		TraceMessage("Saving scene %s. Total entity count: %i", scene.scenePath.c_str(), entityCount);

		WRITE_BINARY(os, entityCount);

		for (auto const& entity : entityList)
		{
			Entity id = entity.second.ID;
			WRITE_BINARY(os, id);

			std::bitset<MAX_COMPONENTS> bitset = entity.second.mask;
			size_t componentSize = componentArrays.size();

			unsigned int componentCount = 0;
			std::vector<unsigned int> componentList;

			for (unsigned int i = 0; i < MAX_COMPONENTS; ++i)
			{
				if (bitset.test(i))
				{
					++componentCount;
					componentList.push_back(i);
				}
			}

			WRITE_BINARY(os, componentCount);

			for (auto& id : componentList)
			{
				WRITE_BINARY(os, id);
				componentArrays[id]->Serialize(entity.second.ID, os);
			}
		}

		std::unordered_map<Entity, std::vector<Behavior*>> behaviorMap = behaviorManager.GetMap();

		size_t size = behaviorMap.size();
		WRITE_BINARY(os, size);

		for (auto const& index : behaviorMap)
		{
			auto& behaviorVector = index.second;
			WRITE_BINARY(os, index.first);

			for (auto behavior : behaviorVector)
			{
				behavior->Serialize(os);
			}
		}

		TraceMessage("Scene with name %s has been saved successfully.", scene.sceneName.c_str());

		os.close();
	}

	#pragma endregion

	#pragma region EntityManagement

	Entity Instantiate()
	{
		return entityFactory.Instantiate();
	}

	Entity GetEntity(const unsigned int index)
	{
		return entityFactory[index].ID;
	}

	void Destroy(const Entity& entity)
	{
		if (entityFactory[entity].ID == -1) return;

		Entity newEntity = -1;

		for (int i = 0; i < MAX_COMPONENTS; ++i)
		{
			if (entityFactory[entity].mask.test(i))
			{
				componentManager[i]->Remove(entity);
			}
		}

		entityFactory.Destroy(entity);
	}

	void PrintAllEntities()
	{
		entityFactory.PrintAll();
	}

	#pragma endregion

	#pragma region ComponentManagement

	template <typename T>
	T* AddComponent(const Entity& entity)
	{
		entityFactory[entity].mask.set(GetComponentID<T>(), true);

		systemManager.AddToSystem(entityFactory[entity]);

		return componentManager.AddComponent<T>(entity);
	}

	template <typename T>
	T* GetComponent(const Entity& entity)
	{
		return &componentManager.GetComponent<T>(entity);
	}

	template <typename T>
	void RemoveComponent(const Entity& entity)
	{
		auto& entityInfo = entityFactory.GetEntityInfo(entity);
		systemManager.RemoveFromSystem(entityInfo);
		entityInfo.mask.set(GetComponentID<T>(), false);
		componentManager.RemoveComponent<T>(entity);
	}

	template <typename T>
	void RegisterComponent()
	{
		componentReconstructMap.insert
		({
			GetComponentID<T>(),
			[&](const Entity& entity, std::istream& is) {
				AddComponent<T>(entity)->DeSerialize(is);
			}
		});

		componentManager.CreateComponentArray<T>();
	}

	#pragma endregion

	#pragma region Behaviors

	template <typename T>
	T* AddBehavior(const Entity& entity)
	{
		return behaviorManager.AddBehavior<T>(entity);
	}

	template <typename T>
	T* GetBehavior(const Entity& entity)
	{
		return behaviorManager.GetBehavior<T>(entity);
	}

	template <typename T>
	T* FindBehavior()
	{
		return behaviorManager.FindBehavior<T>();
	}

	template <typename T>
	void RemoveBehavior(const Entity& entity)
	{
		behaviorManager.RemoveBehavior<T>(entity);
	}

	template <typename T>
	void RegisterBehavior()
	{
		behaviorManager.RegisterBehavior<T>();
	}

	#pragma endregion

	#pragma region Systems

	void AddToSystem(const Entity& entity)
	{
		systemManager.AddToSystem(entityFactory[entity]);
	}

	template <typename SystemType>
	void RegisterSystem()
	{
		System* system = new SystemType(componentManager);
		systemManager.RegisterSystem(system);
	}

	void Update()
	{
		systemManager.UpdateSystems();
		behaviorManager.Update();
	}

	void FixedUpdate()
	{
		behaviorManager.FixedUpdate();
	}

	#pragma endregion

	void Clear()
	{
		componentManager.Clear();
		systemManager.Clear();
		behaviorManager.Clear();
		entityFactory.Clear();
	}

private:
	EntityFactory    entityFactory;
	ComponentManager componentManager;
	SystemManager    systemManager;
	BehaviorManager  behaviorManager;

	std::vector<Scene> scenes;

	const char* sceneFile = "sceneRegistry.txt";

	std::unordered_map<unsigned int, std::function<void(const Entity&, std::istream&)>> componentReconstructMap;
};