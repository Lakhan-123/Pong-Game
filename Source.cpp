#include "pong23.h";

const int thickness = 10.0f;
const float paddleH = 100.0f;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mIsRunning(true)
,mTicksCount(0)
,mPaddleDir(0)
,rPaddleDir(0){

}

bool Game::Initialize(){
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0){
		SDL_Log("Unable to Initialize SDL: %s", SDL_GetError());
		return false;
	}

	auto flags = 0;
	mWindow = SDL_CreateWindow(
		"PonG || plaY wiTh baLL",
		100, 100, 1024, 786,
		flags
	);
	if (!mWindow){
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!mRenderer){
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f / 2.0f;
	rPaddlePos.x = 999.0f;
	rPaddlePos.y = 768.0f / 2.0f;
	mBallPos.x = 1024.0f / 2.0f;
	mBallPos.y = 768.0f / 2.0f;
	mBallVel.x = -200.0f;
	mBallVel.y = 235.0f;

	return true;
}

void Game::RunLoop(){
	while (mIsRunning){
		ProcessInput();

		UpdateGame();

		GenerateOutput();
	}
}

void Game::ProcessInput(){
	SDL_Event event;
	while (SDL_PollEvent(&event)){
		switch (event.type){
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE]) {
		mIsRunning = false;
	}

	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W]) {
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S]) {
		mPaddleDir += 1;
	}

	rPaddleDir = 0;
	if (state[SDL_SCANCODE_I]) {
		rPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_K]) {
		rPaddleDir += 1;
	}
}

void Game::UpdateGame(){
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	if (deltaTime > 0.05f)
		deltaTime = 0.05f;

	mTicksCount = SDL_GetTicks();

	if (mPaddleDir != 0){
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		if (mPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos.y > (785.0f - paddleH / 2.0f - thickness))// 785 for the gap between lower wall and paddle.
		{
			mPaddlePos.y = 785.0f - paddleH / 2.0f - thickness;
		}
	}

	//right padlle
	if (rPaddleDir != 0) {
		rPaddlePos.y += rPaddleDir * 300.0f * deltaTime;
		if (rPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			rPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (rPaddlePos.y > (785.0f - paddleH / 2.0f - thickness))
		{
			rPaddlePos.y = 785.0f - paddleH / 2.0f - thickness;
		}
	}

	mBallPos.x += mBallVel.x * (1 * deltaTime);
	mBallPos.y += mBallVel.y * (1 * deltaTime);

	float diff = mPaddlePos.y - mBallPos.y;
	diff = (diff > 0.0f) ? diff : -diff;

	float rdiff = rPaddlePos.y - mBallPos.y;
	rdiff = (rdiff > 0.0f) ? rdiff : -rdiff;

	if (diff <= paddleH / 2.0f && mBallPos.x <= 25.0f && mBallPos.x >= 20.0f && mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
	}
	else if (rdiff <= paddleH / 2.0f && mBallPos.x >= rPaddlePos.x && mBallPos.x <= rPaddlePos.x + 20.0f)
	{
		mBallVel.x *= -1.0f;
	}
	// Did the ball go off the screen? (if so, end game)
	else if (mBallPos.x <= 0.0f)
	{
		mIsRunning = false;
	}
	// Did the ball collide with the right wall? (if so, end game)
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f)
	{
		mIsRunning = false;
	}
	// Did the ball collide with the top wall?
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		mBallVel.y *= -1;
	}
	// Did the ball collide with the bottom wall?
	else if (mBallPos.y >= (768) && mBallVel.y > 0.0f)
	{
		mBallVel.y *= -1;
	}	
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mRenderer, 128, 128, 128, 255);

	SDL_RenderClear(mRenderer);

	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);

	//upper wall
	SDL_Rect Wall{
		0,	//Top Left x
		0,	//Top Left y
		1024,	 //width
		thickness//hieght
	};
	SDL_RenderFillRect(mRenderer, &Wall);

	//lower wall
	Wall.y = 786 - thickness;
	SDL_RenderFillRect(mRenderer, &Wall);

	//middle wall
	Wall.x = 1024 / 2;
	Wall.y = 0.0f;
	Wall.w = 3.0f;
	Wall.h = 786.0f;
	SDL_RenderFillRect(mRenderer, &Wall);

	//left paddle
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	//right padlle
	SDL_Rect rpaddle{
		static_cast<int>(rPaddlePos.x),
		static_cast<int>(rPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &rpaddle);
	
	SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);

	SDL_Rect ball{
		static_cast<int>(mBallPos.x - thickness / 2),
		static_cast<int>(mBallPos.y - thickness / 2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);

	SDL_RenderPresent(mRenderer);
}

void Game::ShutDown()
{
	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);

	SDL_Quit();
}