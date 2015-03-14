#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include <SDL.h>
#include <vector>
#include <IMouseListener.h>

/*! Manages user input and notifies listeners when users perform actions
*
*/
class InputManager
{
public:
	// returns singleton instance
	static InputManager* GetInstance();

	void OnEvent(SDL_Event* in_Event);

	/*! Registers a new mouse observer */
	void AddMouseListener(IMouseListener* mouseListener);

	/*! Deregisters a mouse observer */
	void RemoveMouseListener(IMouseListener* mouseListener);

private:
	InputManager();
	virtual ~InputManager();

	// Singleton
	static InputManager Instance;

	// vector that hold all mouse observers
	std::vector<IMouseListener*> m_MouseListener;

};

#endif

