#include "GameLoop.h"
#include "SDL.h"
#include <iostream>
#include "ChessUser.h"
#include <utility>

std::map<GameLoop::Piece, const char*> GameLoop::m_pieceMapB =
{
	{ GameLoop::Piece::Pawn, "Assets/Images/b_pawn.png" },
	{ GameLoop::Piece::Rook, "Assets/Images/b_rook.png" },
	{ GameLoop::Piece::Horse, "Assets/Images/b_knight.png" },
	{ GameLoop::Piece::Bishop, "Assets/Images/b_bishop.png" },
	{ GameLoop::Piece::King, "Assets/Images/b_king.png" },
	{ GameLoop::Piece::Queen, "Assets/Images/b_queen.png" },
};

std::map<GameLoop::Piece, const char*> GameLoop::m_pieceMapW =
{
	{ GameLoop::Piece::Pawn, "Assets/Images/w_pawn.png" },
	{ GameLoop::Piece::Rook, "Assets/Images/w_rook.png" },
	{ GameLoop::Piece::Horse, "Assets/Images/w_knight.png" },
	{ GameLoop::Piece::Bishop, "Assets/Images/w_bishop.png" },
	{ GameLoop::Piece::King, "Assets/Images/w_king.png" },
	{ GameLoop::Piece::Queen, "Assets/Images/w_queen.png" },
};

GameLoop::GameLoop() :
	m_pGameWindow(nullptr),
	m_pRenderer(nullptr),		
	m_bIsRunning(1),
	m_bPadding(0),
	m_iFrameCount(0)
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

		//SDL_RenderClear(m_pRenderer);
		//SDL_RenderPresent(m_pRenderer);
		//SDL_Delay(3000);

		m_bIsRunning = true;
	}
	else
	{
		m_bIsRunning = false;
		return;
	}

	InitChess();
	InitPlayerAndAI();

}

void GameLoop::InitChess()
{
	if (SDL_Renderer* pRenderer = m_pRenderer)
	{
		auto CreateLightSquare = [&pRenderer]() -> const NMSprite*
		{
			const NMSprite* backgroundTileLight = new NMSprite(pRenderer, "Assets/Images/square brown light.png");
			return backgroundTileLight;
		};
		auto CreateDarkSquare = [&pRenderer]() -> const NMSprite*
		{
			const NMSprite* backgroundTileDark = new NMSprite(pRenderer, "Assets/Images/square brown dark.png");
			return backgroundTileDark;
		};

		for (const NMSprite* pSprite : m_pBackgroundTiles)
		{
			if (pSprite)
			{
				delete pSprite;
				pSprite = nullptr;
			}
		}
		m_pBackgroundTiles.clear();

		const int rows = 8;
		const int columns = 8;
		bool light = false;
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns ; j++)
			{

				if (light)
				{
					const NMSprite* lightSquare = CreateLightSquare();
					m_pBackgroundTiles.push_back(lightSquare);
					light = false;
				}
				else
				{
					const NMSprite* darkSquare = CreateDarkSquare();
					m_pBackgroundTiles.push_back(darkSquare);
					light = true;
				}
			}
			light = !light;
		}
	}
}

void GameLoop::InitPlayerAndAI()
{
	if (SDL_Renderer* pRenderer = m_pRenderer)
	{
		for (const NMSprite* pSprite : m_pWhitePieces)
		{
			if (pSprite)
			{
				delete pSprite;
				pSprite = nullptr;
			}
		}
		m_pWhitePieces.clear();

		// pawn x8
		// rook/horse/bishop/queen/king/bishop/horse/rook
		for (int i = 0; i < 8 ; i++)
		{
			m_pWhitePieces.push_back(CreatePiece(Piece::Pawn, Side::WHITE));
		}

		m_pWhitePieces.push_back(CreatePiece(Piece::Rook, Side::WHITE));
		m_pWhitePieces.push_back(CreatePiece(Piece::Horse, Side::WHITE));
		m_pWhitePieces.push_back(CreatePiece(Piece::Bishop, Side::WHITE));
		m_pWhitePieces.push_back(CreatePiece(Piece::Queen, Side::WHITE));
		m_pWhitePieces.push_back(CreatePiece(Piece::King, Side::WHITE));
		m_pWhitePieces.push_back(CreatePiece(Piece::Bishop, Side::WHITE));
		m_pWhitePieces.push_back(CreatePiece(Piece::Horse, Side::WHITE));
		m_pWhitePieces.push_back(CreatePiece(Piece::Rook, Side::WHITE));

		for (const NMSprite* pSprite : m_pBlackPieces)
		{
			if (pSprite)
			{
				delete pSprite;
				pSprite = nullptr;
			}
		}
		m_pBlackPieces.clear();

		// pawn x8
		// rook/horse/bishop/queen/king/bishop/horse/rook
		m_pBlackPieces.push_back(CreatePiece(Piece::Rook, Side::BLACK));
		m_pBlackPieces.push_back(CreatePiece(Piece::Horse, Side::BLACK));
		m_pBlackPieces.push_back(CreatePiece(Piece::Bishop, Side::BLACK));
		m_pBlackPieces.push_back(CreatePiece(Piece::Queen, Side::BLACK));
		m_pBlackPieces.push_back(CreatePiece(Piece::King, Side::BLACK));
		m_pBlackPieces.push_back(CreatePiece(Piece::Bishop, Side::BLACK));
		m_pBlackPieces.push_back(CreatePiece(Piece::Horse, Side::BLACK));
		m_pBlackPieces.push_back(CreatePiece(Piece::Rook, Side::BLACK));

		for (int i = 0; i < 8; i++)
		{
			m_pBlackPieces.push_back(CreatePiece(Piece::Pawn, Side::BLACK));
		}
	}
}

const NMSprite* GameLoop::CreatePiece(Piece param1, Side side)
{
	NMSprite* pSprite = nullptr;
	if (side == Side::WHITE)
	{
		pSprite = new NMSprite(m_pRenderer, m_pieceMapW[param1]);
	}
	else
	{
		pSprite = new NMSprite(m_pRenderer, m_pieceMapB[param1]);		
	}
	return pSprite;
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
	for (const NMSprite*& pSprite : m_pBackgroundTiles)
	{
		if(!pSprite)
			continue;

		if (SDL_Texture* pTexture = pSprite->GetTexture())
		{
			SDL_Rect drawRect;
			drawRect.h = 128;
			drawRect.w = 128;
			drawRect.x = xOffset + ( x * 128 );
			drawRect.y = yOffset + ( y * 128 );
			SDL_RenderCopy(m_pRenderer, pTexture, nullptr, &drawRect);
			x++;
			if (x >= columns)
			{
				x = 0;
				y++;
			}
		}
	}

	x = 0;
	y = 6;	
	int innerTileOffsetX = 16;
	const int innerTileOffsetY = 16;
	for (const NMSprite*& pSprite : m_pWhitePieces)
	{
		if (!pSprite)
			continue;

		if (SDL_Texture* pTexture = pSprite->GetTexture())
		{
			SDL_Rect drawRect;
			drawRect.h = 96;
			drawRect.w = y == 7 ? 96 : 76;
			innerTileOffsetX = y == 7 ? 16 : 26;
			drawRect.x = xOffset + innerTileOffsetX + (x * 128);
			drawRect.y = yOffset + innerTileOffsetY + (y * 128);
			SDL_RenderCopy(m_pRenderer, pTexture, nullptr, &drawRect);
			x++;
			if (x >= columns)
			{
				x = 0;
				y++;
			}
		}
	}

	x = 0;
	y = 0;
	for (const NMSprite*& pSprite : m_pBlackPieces)
	{
		if (y >= 2)
			break;

		if (!pSprite)
			continue;

		if (SDL_Texture* pTexture = pSprite->GetTexture())
		{
			SDL_Rect drawRect;
			drawRect.h = 96;
			drawRect.w = 96;

			drawRect.w = y == 0 ? 96 : 76;
			innerTileOffsetX = y == 0 ? 16 : 26;

			drawRect.x = xOffset + innerTileOffsetX + (x * 128);
			drawRect.y = yOffset + innerTileOffsetY + (y * 128);
			SDL_RenderCopy(m_pRenderer, pTexture, nullptr, &drawRect);
			x++;
			if (x >= columns)
			{
				x = 0;
				y++;
			}
		}
	}

	SDL_RenderPresent(m_pRenderer);
}

void GameLoop::AddPlayer(ChessUser* user, bool isWhite)
{
	Side side = isWhite ? Side::WHITE : Side::BLACK;
	m_pPlayer = { user, side };
}

void GameLoop::AddAI(ChessUser* ai, bool isWhite)
{
	Side side = isWhite ? Side::WHITE : Side::BLACK;
	m_pAI = { ai, side };
}
