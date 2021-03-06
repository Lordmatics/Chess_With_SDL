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
#include "SDL_events.h"

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

	static int s_tileSize;
	static int s_width;
	static int s_height;

	static int s_xOffset;
	static int s_yOffset;
	
	static int s_pieceSize;
	static int s_pawnSize;
	static int s_pieceOffset;
	static int s_pawnOffset;
private:
	void ConstructSDL(int w = 1920, int h = 1080, bool fullscreen = false);

	void OnMouseDown(const SDL_Event& event);
	void OnMouseUp(const SDL_Event& event);
	void OnMouse(const SDL_Event& event);

	SDL_Window* m_pGameWindow;
	SDL_Renderer* m_pRenderer;
	SDL_Texture* m_pHighlightedTex;
	bool m_bIsRunning : 1;
	bool m_bPadding : 7;
	int m_iFrameCount;

	//bool m_bIgnorePress;
	Board m_board;

	bool m_LMBD;
	//bool m_playersTurn;
	SDL_Point m_mousePosition;
	//SDL_Rect* m_pSelectedRect;
	//SDL_Point m_clickOffset;

	//SDL_Rect m_resetPos;
	//Tile* m_pPiecesTile;
	//Piece* m_pSelectedPiece;
	//void ClearInput();
};

