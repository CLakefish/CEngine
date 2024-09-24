#pragma once
#include <iostream>
#include "BehaviorManager.h"

class BehaviorTest : public Behavior
{
public:
	void Enter() override
	{
		std::cout << "wowee!" << std::endl;
	}

	void Update() override
	{
		std::cout << "Updated!" << std::endl;
	}

	void FixedUpdate() override
	{
		std::cout << "Fixed Updated!" << std::endl;
	}

	void Exit() override
	{
		std::cout << "Exiting!" << std::endl;
	}

	void Serialize(std::ostream& os) override
	{
		
	}

	void DeSerialize(std::istream& is) override
	{

	}
};