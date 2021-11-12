#pragma once
#include<iostream>
#include<vector>
#include<SDL.h>

struct Vector2 {
	float x;
	float y;
};

struct Ball {
	float pos;
	float vel;
};

class Game {
public:

	Game();

	bool Initialize();
	void RunLoop();
	void ShutDown();

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	Uint32 mTicksCount;

	int mPaddleDir;
	int rPaddleDir;

	bool mIsRunning;

	Vector2 mPaddlePos;
	Vector2 rPaddlePos;

	Vector2 mBallPos;
	Vector2 mBallVel;

};
