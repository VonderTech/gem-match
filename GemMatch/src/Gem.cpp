#include "Gem.h"

Gem::Gem(Color color) :
	m_Color(color),
	m_State(State::Normal),
	m_IsInUse(false)
{

}

Gem::~Gem()
{

}

void Gem::SetSprite(Sprite* const sprite)
{
	m_Sprite = sprite;
}

void Gem::SetState(State newState)
{
	m_State = newState;
}

Gem::State const& Gem::GetState()
{
	return m_State;
}

void Gem::Render()
{
	if (m_State == State::Highlighted)
	{
		m_Sprite->SetColor(200,200,200);
	}
	else
	{
		m_Sprite->SetColor(255, 255, 255);
	}

	m_Sprite->Render();
}