#ifndef __IMOUSE_LISTENER_H__
#define __IMOUSE_LISTENER_H__

#include <SDL.h>

/*! Interface to listen to SDL Mouse events
*
*/
class IMouseListener
{
public:
	virtual ~IMouseListener() {}
	/*! Get called when user moves the mouse */
	virtual void OnMouseMove(const uint32_t& mouseX, const uint32_t& mouseY) = 0;
	/*! Get called when user presses a mouse button */
	virtual void OnMouseDown(SDL_Event* in_Event) = 0;
	/*! Get called when user releases a mouse button */
	virtual void OnMouseReleased(SDL_Event* in_Event) = 0;
};
#endif