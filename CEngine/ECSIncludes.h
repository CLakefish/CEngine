#pragma once
#include <bitset> 
#include <unordered_map>
#include "Trace.h"

const std::int16_t MAX_COMPONENTS = 12;
const size_t       MAX_ENTITIES   = 500;

typedef std::int32_t				Entity;
typedef std::bitset<MAX_COMPONENTS> EntityMask;

struct EntityInfo
{
	Entity ID;
	EntityMask mask;
};

extern unsigned int componentCount = 0;

template <class T>
int GetComponentID()
{
	static int id = componentCount++;
	return id;
}