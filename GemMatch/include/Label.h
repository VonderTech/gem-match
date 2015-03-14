#ifndef __LABEL_H__
#define __LABEL_H__

#include <SDL_ttf.h>
#include <string>

/*! A text label that renders a True Type Font
*
*/
class Label
{
public:
	Label(const int& size);
	virtual ~Label();

	/*! Sets label text */
	void SetText(const std::string& text);

	/*! Sets label position */
	void SetPosition(const int &x, const int &y);

	/*! Sets label color */
	void SetColor(const uint8_t &r, const uint8_t &g, const uint8_t &b);

	/*! Renders label */
	void Render();

private:
	// font size in points
	int m_Size;
	// true type font
	TTF_Font* m_Font;
	SDL_Color m_Color;
	std::string m_Text;
	SDL_Renderer* m_Renderer = nullptr;
	SDL_Surface *m_Surface = nullptr;
	SDL_Texture *m_Texture = nullptr;
	SDL_Rect m_Rect;
};

#endif

