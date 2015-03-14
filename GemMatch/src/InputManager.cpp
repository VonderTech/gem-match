#include <Log.h>
#include <algorithm>
#include "InputManager.h"

// ensure there is only a single instance of the class.
InputManager InputManager::Instance;

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
	m_MouseListener.clear();
}

void InputManager::AddMouseListener(IMouseListener* mouseListener)
{
	m_MouseListener.push_back(mouseListener);
}

void InputManager::RemoveMouseListener(IMouseListener* mouseListener)
{
	// find listener
	std::vector<IMouseListener*>::iterator position = std::find(m_MouseListener.begin(), m_MouseListener.end(), mouseListener);

	// if listener has been found, remove it 
	if (position != m_MouseListener.end())
		m_MouseListener.erase(position);
}

void InputManager::OnEvent(SDL_Event* in_Event)
{
	if (in_Event->type == SDL_MOUSEMOTION)
	{
		for (auto listener : m_MouseListener)
		{
			listener->OnMouseMove(in_Event->motion.x, in_Event->motion.y);
		}
	}

	if (in_Event->type == SDL_MOUSEBUTTONUP)
	{
		for (auto listener : m_MouseListener)
		{
			listener->OnMouseReleased(in_Event);
		}
	}

	if (in_Event->type == SDL_MOUSEBUTTONDOWN)
	{
		for (auto listener : m_MouseListener)
		{
			listener->OnMouseDown(in_Event);
		}
	}
}

InputManager* InputManager::GetInstance()
{
	return &InputManager::Instance;
}

