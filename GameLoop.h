#pragma once
#include "NMSprite.h"
#include <vector>
#include <map>
#include "SDL_rect.h"
#include "apiObject.h"
#include "Board.h"
#include "ChessUser.h"
#include "Player.h"
#include "BasicAI.h"

struct SDL_Window;
struct SDL_Renderer;
class GameLoop
{
public:

	GameLoop();
	GameLoop(int width, int height);
	virtual ~GameLoop();

	bool IsRunning() const;
	void CleanUp();

	void InitGame();
	void HandleEvents();
	void Process(float dt);
	void Render();

private:
	void ConstructSDL(int w = 1920, int h = 1080, bool fullscreen = false);

	SDL_Window* m_pGameWindow;
	SDL_Renderer* m_pRenderer;
	SDL_Texture* m_pHighlightedTex;
	bool m_bIsRunning : 1;
	bool m_bPadding : 7;
	int m_iFrameCount;

	bool m_bIgnorePress;
	bool m_bPlayerIsWhite;
	Board m_board;

	bool m_LMBD;
	bool m_playersTurn;
	SDL_Point m_mousePosition;
	SDL_Rect* m_pSelectedRect;
	SDL_Point m_clickOffset;

	SDL_Rect m_resetPos;
	Tile* m_pPiecesTile;
	Piece* m_pSelectedPiece;
	void ClearInput();
};

