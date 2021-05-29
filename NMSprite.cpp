#include "NMSprite.h"
#include "SDL_surface.h"
#include "SDL_render.h"
#include "SDL_image.h"
#include <iostream>

NMSprite::NMSprite() :
	m_spriteResourceFilePath("")
{

}

NMSprite::NMSprite(SDL_Renderer* pRenderer, const char* path) :
	m_spriteResourceFilePath(path)
{
	AssignSprite(pRenderer, path);
}

NMSprite::~NMSprite()
{	
	SDL_DestroyTexture(m_pTexture);
}

SDL_Texture* NMSprite::GetTexture() const
{
	return m_pTexture;
}

void NMSprite::AssignSprite(SDL_Renderer* pRenderer, const char* path)
{
	SDL_Surface* pTMPSurface = IMG_Load(path);
	if (!pTMPSurface)
	{
		// TODO: ERROR
		std::cout << "Failed to Load Image: " << path << std::endl;
		return;
	}

	if (m_pTexture)
	{
		SDL_DestroyTexture(m_pTexture);
	}

	m_spriteResourceFilePath = path;
	m_pTexture = SDL_CreateTextureFromSurface(pRenderer, pTMPSurface);
	SDL_FreeSurface(pTMPSurface);
}
