#pragma once
#include "ECSIncludes.h"
#include "ECSComponents.h"
#include <unordered_map>
#include <vector>

struct IComponentArray
{
    virtual ~IComponentArray() = default;

    virtual void Remove(const Entity& e) = 0;
    virtual void Clear() = 0;

    virtual bool Serialize(const Entity& e, std::ofstream& os) = 0;
    virtual void Deserialize(const Entity& e, std::ifstream& is) = 0;
};

template <typename T>
struct ComponentArray : IComponentArray
{
    ComponentArray() { }

    ~ComponentArray() override
    {
        Clear();
    }

    T* Add(const Entity& e)
    {
        if (entityToComponentIndex.find(e) != entityToComponentIndex.end())
        {
            return nullptr;
        }

        components.emplace_back();
        entityArray.push_back(e);
        entityToComponentIndex[e] = components.size() - 1;

        return &components.back();
    }

    T* Get(const Entity& e)
    {
        if (entityToComponentIndex.find(e) == entityToComponentIndex.end())
        {
            return nullptr;
        }

        return &components[entityToComponentIndex[e]];
    }

    std::vector<T> GetAllComponents()
    {
        return components;
    }

    std::vector<Entity> GetAllEntities()
    {
        std::vector<Entity> entities;

        entities.reserve(entityToComponentIndex.size());

        for (auto& entity : entityToComponentIndex)
        {
            entities.push_back(entity.first);
        }

        return entities;
    }
    
    void Remove(const Entity& e) override
    {
        auto iter = entityToComponentIndex.find(e);

        if (iter == entityToComponentIndex.end())
        {
            TraceMessage("Component of type ", typeid(T).name(), " was not found on entity with ID ", e);
            return;
        }

        size_t index = iter->second;
        size_t componentSizeIndex = components.size() - 1;

        entityToComponentIndex.erase(iter);

        if (index != componentSizeIndex)
        {
            components[index] = std::move(components.back());
            entityArray[index] = entityArray[componentSizeIndex];

            Entity temp = entityArray[componentSizeIndex];
            entityToComponentIndex[temp] = index;
        }

        components.pop_back();
        entityArray.pop_back();
    }

    void Clear() override
    {
        components.clear();
        entityArray.clear();
        entityToComponentIndex.clear();
    }

    bool Serialize(const Entity& e, std::ofstream& os) override
    {
        if (entityToComponentIndex.find(e) == entityToComponentIndex.end())
        {
            return false;
        }

        components[entityToComponentIndex[e]].Serialize(os);
        return true;
    }

    void Deserialize(const Entity& e, std::ifstream& is) override
    {
        if (entityToComponentIndex.find(e) != entityToComponentIndex.end())
        {
            return;
        }

        T* component = Add(e);
        component->DeSerialize(is);
    }

    std::vector<T> components;
    std::vector<Entity> entityArray;
    std::unordered_map<Entity, size_t> entityToComponentIndex;
};