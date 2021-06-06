#include "GameLoop.h"
#include "SDL.h"
#include <iostream>
#include "ChessUser.h"
#include <utility>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

int GameLoop::s_width = 1920;
int GameLoop::s_height = 1080;
int GameLoop::s_tileSize = 128;
int GameLoop::s_xOffset = ( (1920 - (8 * 128)) / 2); // 1920 / 4; = 
int GameLoop::s_yOffset = ( (1080 - (8 * 128)) / 2); // Creates the gap between top and bottom
int GameLoop::s_pieceSize = 96;
int GameLoop::s_pawnSize = 76;
int GameLoop::s_pawnOffset = 26;
int GameLoop::s_pieceOffset = 16;

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
	s_width = w;
	s_height = h;
	auto Pred = [](int left, int right)
	{
		return left < right;
	};
	s_tileSize = (int)(std::min(w / 8, h / 8, Pred) * 0.975f); // 8.4375f
	s_pieceSize = (int)s_tileSize / 1.3f;
	s_pawnSize = (int)s_tileSize / 1.684210526315789f;
	//s_pawnOffset = (int)(s_width / 8) / 2.153846153846154f;
	//s_pieceOffset = (int)s_width / 8 / 3.5f;
	//const int xOffset = GameLoop::s_width / 4;//  896 / 2; // Quarter X Reso
	//const int yOffset = (GameLoop::s_height - (tileSize * 9)) / 2;// 28; // Each tile is 128, so 128 * 8 = 1024. reso = 1920:1080, so 1080 - 1024 = 56, then half top/bot, so 28 each side
	
	s_xOffset = ((s_width - (8 * s_tileSize)) / 2); // 1920 / 4; = 
	s_yOffset = ((s_height- (8 * s_tileSize)) / 2); // Creates the gap between top and bottom

	s_pawnOffset = (s_tileSize - s_pawnSize) / 2;
	s_pieceOffset = (s_tileSize - s_pieceSize) / 2;

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
