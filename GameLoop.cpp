#include "GameLoop.h"
#include "SDL.h"
#include <iostream>
#include "ChessUser.h"
#include <utility>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

GameLoop::GameLoop() :
	m_pGameWindow(nullptr),
	m_pRenderer(nullptr),		
	m_bIsRunning(1),
	m_bPadding(0),
	m_iFrameCount(0),
	m_board()
{
	ConstructSDL();
}

GameLoop::GameLoop(int width, int height) :
	m_pGameWindow(nullptr),
	m_pRenderer(nullptr),
	m_bIsRunning(1),
	m_bPadding(0),
	m_iFrameCount(0),
	m_board()
{
	ConstructSDL(width, height);
}

GameLoop::~GameLoop()
{
}

bool GameLoop::IsRunning() const
{
	return m_bIsRunning;
}

void GameLoop::ConstructSDL(int w, int h, bool fullscreen)
{
	int flags = 0;
	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0 )
	{
		std::cout << "SDL Initialized!" << std::endl;
		m_pGameWindow = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
		if (!m_pGameWindow)
			return;
		std::cout << "SDL Window Created" << std::endl;

		m_pRenderer = SDL_CreateRenderer(m_pGameWindow, -1, 0);
		if (!m_pRenderer)
			return;
		
		SDL_SetRenderDrawColor(m_pRenderer, 255, 233, 197, 255);
		std::cout << "SDL Renderer Created" << std::endl;
		m_bIsRunning = true;
	}
	else
	{
		m_bIsRunning = false;
		return;
	}

	InitGame();
}

void GameLoop::InitGame()
{
	m_board.Init(m_pRenderer);
}

void GameLoop::CleanUp()
{
	SDL_DestroyWindow(m_pGameWindow);
	std::cout << "Window Destroyed! " << std::endl;
	SDL_DestroyRenderer(m_pRenderer);
	std::cout << "Renderer Destroyed! " << std::endl;
	SDL_Quit();
	std::cout << "Quitting! " << std::endl;

	std::cout << "Game Cleaned!" << std::endl;
}

void GameLoop::HandleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type)
	{
		//https://www.youtube.com/watch?v=jzasDqPmtPI&list=PLhfAbcv9cehhkG7ZQK0nfIGJC_C-wSLrx&index=5
		//https://gigi.nullneuron.net/gigilabs/sdl2-drag-and-drop/
		case SDL_MOUSEBUTTONDOWN:
		{
			OnMouseDown(event);			
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			OnMouseUp(event);
			break;
		}
		case SDL_MOUSEMOTION:
		{
			OnMouse(event);
			break;
		}
		case SDL_QUIT:
		{
			m_bIsRunning = false;
			break;
		}
		default:
			break;
	}
}

void GameLoop::Process(float dt)
{
	m_iFrameCount++;

	m_board.Process(dt);

}

void GameLoop::OnMouseDown(const SDL_Event& event)
{
	if (!m_LMBD && event.button.button == SDL_BUTTON_LEFT)
	{
		m_LMBD = true;

		m_board.OnLeftClickDown(m_pRenderer);
	}
}

void GameLoop::OnMouseUp(const SDL_Event& event)
{
	if (m_LMBD && event.button.button == SDL_BUTTON_LEFT)
	{
		m_LMBD = false;
		m_board.OnLeftClickRelease(m_pRenderer);		
	}
}

void GameLoop::OnMouse(const SDL_Event& event)
{
	m_board.SetMousePosition({ event.motion.x, event.motion.y });

	// TODO: Be nice to change this to an actual texture highlight - white boarder or so.
	if (Tile* pTile = m_board.GetTileAtPoint(&m_mousePosition))
	{
		auto Predicate = [&pTile](Tile* pOtherTile)
		{
			return pTile == pOtherTile;
		};
		if (m_board.TileMatch(Predicate, 0))
		{
			return;
		}
		NMSprite& sprite = pTile->GetSprite();
		if (SDL_Texture* pTexture = sprite.GetTexture())
		{
			if (m_pHighlightedTex && m_pHighlightedTex != pTexture)
			{
				SDL_SetTextureColorMod(m_pHighlightedTex, 255, 255, 255);
			}
			m_pHighlightedTex = pTexture;
		}
	}

	if (m_pHighlightedTex)
	{
		SDL_SetTextureColorMod(m_pHighlightedTex, 178, 178, 178);
	}

}

// https://opengameart.org/content/chess-pieces-and-board-squares
void GameLoop::Render()
{
	SDL_RenderClear(m_pRenderer);

	m_board.Render(m_pRenderer);

	SDL_RenderPresent(m_pRenderer);
}
