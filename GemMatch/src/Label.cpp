#include "Label.h"
#include <Application.h>
#include <Log.h>


Label::Label(const int& size) :
m_Color({ 255, 255, 255 }),
m_Size(size),
m_Renderer(Application::GetInstance()->GetRenderer())
{
	// get path to the main font
	std::string mainFontPath = Application::GetInstance()->GetMainFontPath();

	// open the font
	m_Font = TTF_OpenFont(mainFontPath.c_str(), size);
}



Label::~Label()
{
	// close the font
	TTF_CloseFont(m_Font);

	SDL_DestroyTexture(m_Texture);
}

void Label::SetText(const std::string& text)
{
	m_Text = text;

	// remove previous texture
	if (m_Texture != nullptr)
	{
		SDL_DestroyTexture(m_Texture);
	}

	m_Surface = TTF_RenderText_Blended(m_Font, m_Text.c_str(), m_Color);
	m_Texture = SDL_CreateTextureFromSurface(m_Renderer, m_Surface);
	SDL_FreeSurface(m_Surface);

	// check if texture creation was successful
	if (m_Texture == nullptr)
	{
		Log::GetInstance()->LogSDLError("Could not create texture from surface");
	}
	else
	{
		//Query the texture to get its in_Height and in_Height to use
		SDL_QueryTexture(m_Texture, NULL, NULL, &m_Rect.w, &m_Rect.h);
	}
}

void Label::SetPosition(const int &x, const int &y)
{
	m_Rect.x = x;
	m_Rect.y = y;
}

void Label::SetColor(uint8_t const& red, uint8_t const& green, uint8_t const& blue)
{
	m_Color.r = red;
	m_Color.g = green;
	m_Color.b = blue;

	// remove previous texture
	if (m_Texture != nullptr)
	{
		SDL_DestroyTexture(m_Texture);
	}

	m_Surface = TTF_RenderText_Blended(m_Font, m_Text.c_str(), m_Color);
	m_Texture = SDL_CreateTextureFromSurface(m_Renderer, m_Surface);
	SDL_FreeSurface(m_Surface);
}

void Label::Render()
{
	if (m_Renderer != nullptr && m_Texture != nullptr)
	{
		//render the texture
		SDL_RenderCopy(m_Renderer, m_Texture, NULL, &m_Rect);
	}
}