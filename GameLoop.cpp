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

GameLoop::GameLoop(int width, int height)
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

	m_board.Init(m_pRenderer);


	m_players[0] = &m_player;
	m_players[1] = &m_opponent;	

	std::srand((unsigned int)time(NULL));
	int random = std::rand() % 1;
	if (random == 1)
	{
		m_player.SetWhite(true);
		m_player.SetSide(ChessUser::Side::BOTTOM);
		m_opponent.SetWhite(false);
		m_opponent.SetSide(ChessUser::Side::TOP);
	}
	else
	{
		m_player.SetWhite(false);
		m_player.SetSide(ChessUser::Side::BOTTOM);
		m_opponent.SetWhite(true);
		m_opponent.SetSide(ChessUser::Side::TOP);
	}

	for (int i = 0; i < MAX_NUM_PLAYERS; i++)
	{
		if (m_players[i])
		{
			m_players[i]->Init(m_pRenderer);
		}
	}
}

void GameLoop::CleanUp()
{
	SDL_DestroyWindow(m_pGameWindow);
	SDL_DestroyRenderer(m_pRenderer);
	SDL_Quit();
	std::cout << "Game Cleaned!" << std::endl;
}

void GameLoop::HandleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type)
	{
		/* Drag and drop events */
		//SDL_DROPFILE = 0x1000, /**< The system requests a file open */
		//SDL_DROPTEXT,                 /**< text/plain drag-and-drop event */
		//SDL_DROPBEGIN,                /**< A new set of drops is beginning (NULL filename) */
		//SDL_DROPCOMPLETE,             /**< Current set of drops is now complete (NULL filename) */
		//	/* Keyboard events */
		//SDL_KEYDOWN = 0x300, /**< Key pressed */
		//SDL_KEYUP,                  /**< Key released */
		//SDL_TEXTEDITING,            /**< Keyboard text editing (composition) */
		//SDL_TEXTINPUT,              /**< Keyboard text input */
		//SDL_KEYMAPCHANGED,          /**< Keymap changed due to a system event such as an
		//								 input language or keyboard layout change.
		//							*/
		//
		//							/* Mouse events */
		//SDL_MOUSEMOTION = 0x400, /**< Mouse moved */
		//SDL_MOUSEBUTTONDOWN,        /**< Mouse button pressed */
		//SDL_MOUSEBUTTONUP,          /**< Mouse button released */
		//SDL_MOUSEWHEEL,             /**< Mouse wheel motion */

		//https://www.youtube.com/watch?v=jzasDqPmtPI&list=PLhfAbcv9cehhkG7ZQK0nfIGJC_C-wSLrx&index=5
	//https://gigi.nullneuron.net/gigilabs/sdl2-drag-and-drop/
	case SDL_MOUSEBUTTONDOWN:
	{

		if (!m_LMBD && event.button.button == SDL_BUTTON_LEFT)
		{
			m_LMBD = true;
			//TODO:
			// Declare RECT m_gameRect; (to encompass the board)
			// Declared RECT[][] To represent the grid
			//SDL_PointInRect(&m_mousePosition, rect);
		}

		volatile int i = 5;
		break;
	}
	case SDL_MOUSEBUTTONUP:
	{
		if (m_LMBD && event.button.button == SDL_BUTTON_LEFT)
		{
			m_LMBD = false;
			m_pSelectedRect = nullptr;
			//TODO:
			// Declare RECT m_gameRect; (to encompass the board)
			// Declared RECT[][] To represent the grid
			//SDL_PointInRect(&m_mousePosition, rect);
		}

		volatile int i = 5;
		break;
	}
	case SDL_MOUSEMOTION:
	{
		m_mousePosition = { event.motion.x, event.motion.y };
		break;
	}
	case SDL_DROPFILE:
	{
		volatile int i = 5;
		break;
	}
	case SDL_DROPTEXT:
	{
		volatile int i = 5;
		break;
	}
	case SDL_DROPBEGIN:
	{
		volatile int i = 5;
		break;
	}
		case SDL_DROPCOMPLETE:
		{
			volatile int i = 5;
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

void GameLoop::Process()
{
	m_iFrameCount++;
}

// https://opengameart.org/content/chess-pieces-and-board-squares
void GameLoop::Render()
{
	SDL_RenderClear(m_pRenderer);

	const int rows = 8;
	const int columns = 8;
	int x = 0;
	int y = 0;
	const int xOffset = 896 / 2;
	const int yOffset = 25;

	m_board.Render(m_pRenderer);

	for (int i = 0; i < MAX_NUM_PLAYERS; i++)
	{
		if (m_players[i])
		{
			m_players[i]->Render(m_pRenderer);
		}
	}

	SDL_RenderPresent(m_pRenderer);
}