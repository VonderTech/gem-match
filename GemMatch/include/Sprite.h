#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <SDL.h>
#include <string>

/*! Sets up and draws SDL textures
*
*/
class Sprite
{

public:
	Sprite();
	virtual ~Sprite();

	void Load(const std::string &file);
	void Render();

	/*! Sets sprite position */
	void SetPosition(const int &x, const int &y);

	/*! \brief Sets the width of the sprite
	*
	*	\param in_Width The desired width
	*/
	void SetWidth(const int& in_Width);

	/*! \brief Sets the height of the sprite
	*
	*	\param in_Height The desired height
	*/
	void SetHeight(const int& height);

	/*! Sets the transparency (0 = fully transparent, 255 = opaque)*/
	void SetAlpha(const uint8_t& alpha);

	/*! Tints the sprite with the specified color */
	void SetColor(const uint8_t& red, const uint8_t& green, const uint8_t& blue);

	/*! Retrieves the sprite's width */
	int GetWidth() const;

	/*! Retrieves the sprite's height */
	int GetHeight() const;

	/*! Retrieves the rectangle that hold the sprite's dimensions */
	SDL_Rect Sprite::GetRect() const;

private:
	SDL_Texture* m_Texture = nullptr;
	SDL_Renderer* m_Renderer = nullptr;
	SDL_Rect m_Rect;

};

#endif

