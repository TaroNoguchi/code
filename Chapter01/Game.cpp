// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mPaddleP1Dir(0)
,mPaddleP2Dir(0)
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	//
	mP1PaddlePos.x = 10.0f;
	mP1PaddlePos.y = 768.0f / 2.0f;
	mP2PaddlePos.x = 1014.f;
	mP2PaddlePos.y = 768.0f / 2.0f;
	
	mBallPos.x = 1024.0f/2.0f;
	mBallPos.y = 768.0f/2.0f;
	mBallVel.x = -200.0f;
	mBallVel.y = 235.0f;
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// Update P1 paddle direction based on W/S keys
	mPaddleP1Dir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleP1Dir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleP1Dir += 1;
	}

	// Update P2 paddle direction based on L/K keys
	mPaddleP2Dir = 0;
	if (state[SDL_SCANCODE_L])
	{
		mPaddleP2Dir -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		mPaddleP2Dir += 1;
	}
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	// Update P1 paddle position based on direction
	if (mPaddleP1Dir != 0)
	{
		mP1PaddlePos.y += mPaddleP1Dir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mP1PaddlePos.y < (paddleH/2.0f + thickness))
		{
			mP1PaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mP1PaddlePos.y > (768.0f - paddleH/2.0f - thickness))
		{
			mP1PaddlePos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}

	// Update P2 paddle position based on direction
	if (mPaddleP2Dir != 0)
	{
		mP2PaddlePos.y += mPaddleP2Dir * 300.f * deltaTime;
		// Paddleがスクリーンから消えないように
		if (mP2PaddlePos.y < (paddleH / 2.f + thickness))
		{
			mP2PaddlePos.y = paddleH / 2.f + thickness;
		}
		else if (mP2PaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mP2PaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}
	
	// Update ball position based on ball velocity
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;
	
	// Bounce if needed
	
	// Did we intersect with the P1 paddle?
	float diffToP1 = mP1PaddlePos.y - mBallPos.y;
	// Take absolute value of difference
	diffToP1 = (diffToP1 > 0.0f) ? diffToP1 : -diffToP1;
	
	// P2パドルy座標とボールのy座標, 絶対値化
	float diffToP2 = mP2PaddlePos.y - mBallPos.y;
	diffToP2 = (diffToP2 > 0.f) ? diffToP2 : -diffToP2;

	if (
		// Our y-difference is small enough
		diffToP1 <= paddleH / 2.0f &&
		// We are in the correct x-position
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// The ball is moving to the left
		mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
	}
	else if (
		diffToP2 <= paddleH / 2.f &&
		mBallPos.x <= 1004.f && mBallPos.x >= 999.f &&
		mBallVel.x > 0.0f)
	{
		mBallVel.x *= -1.f;
	}
	// Did the ball go off the screen? (if so, end game)
	// 1P(左) の負け
	else if (mBallPos.x <= 0.0f)
	{
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "2P Win");
		mIsRunning = false;
	}
	// 2P(右) の負け
	else if (mBallPos.x >= 1024.f)
	{
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "1P Win");
		mIsRunning = false;
	}
	
	
	// Did the ball collide with the top wall?
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		mBallVel.y *= -1;
	}
	// Did the ball collide with the bottom wall?
	else if (mBallPos.y >= (768 - thickness) &&
		mBallVel.y > 0.0f)
	{
		mBallVel.y *= -1;
	}
}

void Game::GenerateOutput()
{
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw paddle 1P
	SDL_Rect paddleP1{
		static_cast<int>(mP1PaddlePos.x),
		static_cast<int>(mP1PaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddleP1);
	
	// Draw paddle 2P
	SDL_Rect paddleP2{
		static_cast<int>(mP2PaddlePos.x),
		static_cast<int>(mP2PaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddleP2);

	// Draw ball
	SDL_Rect ball{	
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
