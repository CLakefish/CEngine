#pragma once
#include <iostream>
#include <sstream>

#define WRITE_BINARY(os, var) os.write(reinterpret_cast<const char*>(&var), sizeof(var))
#define READ_BINARY(is, var)  is.read(reinterpret_cast<char*>(&var),  sizeof(var))

template <typename T>
struct Component
{
	void Serialize(std::ostream& os)
	{
		return static_cast<T*>(this)->Serialize(os);
	}

	void DeSerialize(std::istream& is)
	{
		return static_cast<T*>(this)->DeSerialize(is);
	}
};


struct Transform : public Component<Transform>
{
	float x, y, z;

	void Serialize(std::ostream& os)
	{
		WRITE_BINARY(os, x);
		WRITE_BINARY(os, y);
		WRITE_BINARY(os, z);
	}

	void DeSerialize(std::istream& is)
	{
		READ_BINARY(is, x);
		READ_BINARY(is, y);
		READ_BINARY(is, z);
	}
};

struct Rigidbody : public Component<Rigidbody>
{
	void Serialize(std::ostream& os)
	{

	}

	void DeSerialize(std::istream& is)
	{

	}
};

struct Mesh : public Component<Mesh>
{
	std::string filePath;

	void Serialize(std::ostream& os)
	{

	}

	void DeSerialize(std::istream& is)
	{

	}
};

struct BoxCollider : public Component<BoxCollider>
{
	void Serialize(std::ostream& os)
	{

	}

	void DeSerialize(std::istream& is)
	{

	}
};

struct SphereCollider : public Component<SphereCollider>
{
	float radius;

	void Serialize(std::ostream& os)
	{

	}

	void DeSerialize(std::istream& is)
	{

	}
};

struct CapsuleCollider : public Component<CapsuleCollider>
{
	float radius;

	void Serialize(std::ostream& os)
	{

	}

	void DeSerialize(std::istream& is)
	{

	}
};

struct MeshCollider : public Component<MeshCollider>
{
	void Serialize(std::ostream& os)
	{

	}

	void DeSerialize(std::istream& is)
	{

	}
};

struct ParticleSystem : public Component<ParticleSystem>
{
	void Serialize(std::ostream& os)
	{

	}

	void DeSerialize(std::istream& is)
	{
		
	}
};

struct AudioSource : public Component<AudioSource>
{
	void Serialize(std::ostream& os)
	{

	}

	void DeSerialize(std::istream& is)
	{

	}
};

struct LightSource : public Component<LightSource>
{
	void Serialize(std::ostream& os)
	{

	}

	void DeSerialize(std::istream& is)
	{

	}
};