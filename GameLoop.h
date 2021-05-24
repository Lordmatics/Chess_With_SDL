#pragma once
#include "NMSprite.h"
#include <vector>
#include <map>
#include "SDL_rect.h"

struct SDL_Window;
struct SDL_Renderer;
class ChessUser;
class GameLoop
{
public:

	GameLoop();
	GameLoop(int width, int height);
	virtual ~GameLoop();

	bool IsRunning() const;
	void CleanUp();

	void HandleEvents();
	void Process();
	void Render();

	void AddPlayer(ChessUser* user, bool isWhite = true);
	void AddAI(ChessUser* ai, bool isWhite = false);

	enum Piece
	{
		Pawn,
		Rook,
		Horse,
		Bishop,
		Queen,
		King,
	};

	enum Side
	{
		WHITE,
		BLACK
	};

private:
	void ConstructSDL(int w = 1920, int h = 1080, bool fullscreen = false);
	void InitChess();

	SDL_Window* m_pGameWindow;
	SDL_Renderer* m_pRenderer;
	bool m_bIsRunning : 1;
	bool m_bPadding : 7;
	int m_iFrameCount;

	std::pair<ChessUser*, Side> m_pPlayer;
	std::pair<ChessUser*, Side> m_pAI;

	std::vector<const NMSprite*> m_pBlackPieces;
	std::vector<const NMSprite*> m_pWhitePieces;
	std::vector<const NMSprite*> m_pBackgroundTiles;
	static std::map<Piece, const char*> m_pieceMapB;
	static std::map<Piece, const char*> m_pieceMapW;

	bool m_LMBD;
	SDL_Point m_mousePosition;
	SDL_Rect* m_pSelectedRect;
	SDL_Point m_clickOffset;

	void InitPlayerAndAI();
	const NMSprite* CreatePiece(Piece param1, Side side);
};

