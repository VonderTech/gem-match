#include <SDL.h>
#include <SDL_image.h>
#include "Application.h"
#include "Log.h"
#include "Sprite.h"

Sprite::Sprite() :
m_Renderer(Application::GetInstance()->GetRenderer())
{

}

Sprite::~Sprite()
{
	if (m_Texture != nullptr)
	{
		SDL_DestroyTexture(m_Texture);
	}
}

void Sprite::Load(const std::string &file)
{
	
	m_Texture = IMG_LoadTexture(m_Renderer, file.c_str());

	// check if texture creation was successful
	if (m_Texture == nullptr)
	{
		Log::GetInstance()->LogSDLError("Could not create texture from surface");
	}
	else
	{
		//Query the texture to get its height and height to use
		SDL_QueryTexture(m_Texture, NULL, NULL, &m_Rect.w, &m_Rect.h);
	}
}

void Sprite::Render()
{
	//render the texture
	SDL_RenderCopy(m_Renderer, m_Texture, NULL, &m_Rect);
}

void Sprite::SetPosition(const int &x, const int &y)
{
	m_Rect.x = x;
	m_Rect.y = y;
}

void Sprite::SetWidth(const int& height)
{
	m_Rect.w = height;

}

void Sprite::SetHeight(const int& height)
{
	m_Rect.h = height;
}


void Sprite::SetAlpha(uint8_t const& alpha)
{
	SDL_SetTextureAlphaMod(m_Texture, alpha);
}

void Sprite::SetColor(const uint8_t& red, const uint8_t& green, const uint8_t& blue)
{
	SDL_SetTextureColorMod(m_Texture, red, green, blue);
}

int Sprite::GetWidth() const
{
	return m_Rect.w;
}

int Sprite::GetHeight() const
{
	return m_Rect.h;
}

SDL_Rect Sprite::GetRect() const
{
	return m_Rect;
}