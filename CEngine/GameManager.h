#pragma once
#include "SceneManager.h"

class GameManager
{
public:
	void Init()
	{
		running = true;

		TraceInit();
		sceneManager.Init();

		for (unsigned int i = 0; i < 1000; ++i)
		{
			Entity e = sceneManager.Instantiate();

			sceneManager.AddComponent<Transform>(e);
			sceneManager.AddComponent<Mesh>(e);
			sceneManager.AddComponent<Rigidbody>(e);
		}

		sceneManager.SaveScene();

		sceneManager.LoadScene(0);

		TraceClose();
	}

private:
	SceneManager sceneManager;
	bool running = false;
};