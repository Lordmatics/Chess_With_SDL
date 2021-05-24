#include "SDL.h"
#include "GameLoop.h"
#include "ChessUser.h"
#include "Player.h"
#include "BasicAI.h"

int main(int argc, char* argv[])
{
	GameLoop* pGameLoop = new GameLoop();
	
	bool playerIsWhite = true;
	Player* player = new Player();
	pGameLoop->AddPlayer(player, playerIsWhite);
	
	BasicAI* ai = new BasicAI();
	pGameLoop->AddAI(ai, !playerIsWhite);
	
	const int FPS = 60;
	const int frameExpectation = 1000 / FPS;
	uint32_t frameStart;
	int frameTime;

	while (pGameLoop->IsRunning())
	{
		frameStart = SDL_GetTicks();
		pGameLoop->HandleEvents();
		pGameLoop->Process();
		pGameLoop->Render();		

		frameTime = SDL_GetTicks() - frameStart;
		
		// If not enough time has passed to cover the frame, wait for the remainder
		if (frameExpectation > frameTime)
		{
			SDL_Delay(frameExpectation - frameTime);
		}
	};

	pGameLoop->CleanUp();
	delete pGameLoop;
	pGameLoop = nullptr;

	return 0;
}