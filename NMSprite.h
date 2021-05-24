#pragma once

struct SDL_Texture;
struct SDL_Renderer;
class NMSprite
{
public:
	NMSprite(SDL_Renderer* pRenderer, const char* path);
	virtual ~NMSprite();
	SDL_Texture* GetTexture() const;
private:
	NMSprite() = delete;

	SDL_Texture* m_pTexture;
};

