#pragma once

struct SDL_Texture;
struct SDL_Renderer;
class NMSprite
{
public:
	NMSprite();
	NMSprite(SDL_Renderer* pRenderer, const char* path);
	virtual ~NMSprite();
	SDL_Texture* GetTexture() const;
	void AssignSprite(SDL_Renderer* pRenderer, const char* path);
private:

	SDL_Texture* m_pTexture;
};

