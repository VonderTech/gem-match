#include <SDL.h>
#include <string>
#include <iostream>
#include "Sprite.h"
#include "Label.h"
#include "CountdownTimer.h"

CountdownTimer::CountdownTimer(uint32_t startTime) :
m_Time(startTime),
m_StartTime(startTime),
m_StartTicks(0),
m_Interval(1000),
m_HasStarted(false),
m_Label(new Label(60))
{
	// adjust countdown label position
	m_Label->SetPosition(80, 30);
	m_Label->SetText(std::to_string(startTime));
}

CountdownTimer::~CountdownTimer()
{
	delete m_Label;
}


void CountdownTimer::SetInterval(uint32_t const& interval)
{
	m_Interval = interval;
}

void CountdownTimer::SetFinishedHandler(std::function<void()> handlerFunction)
{
	m_CountdownFinishEvent = handlerFunction;
}

void CountdownTimer::Start()
{
	m_StartTicks = SDL_GetTicks();
	m_HasStarted = true;
}


void CountdownTimer::Stop()
{
	m_HasStarted = false;
}

void CountdownTimer::Reset()
{
	m_Time = m_StartTime;
}

void CountdownTimer::Update()
{
	if (m_HasStarted)
	{
		Uint32 deltaTime = SDL_GetTicks() - m_StartTicks;

		if (deltaTime > m_Interval)
		{
			if (m_Time == 0)
			{
				Stop();

				// callback handler
				if (m_CountdownFinishEvent != nullptr)
				{
					m_CountdownFinishEvent();
				}
			}
			else
			{
				// subtract interval
				m_Time -= m_Interval / 1000;

				// update label
				std::string labelText = std::to_string(m_Time);
				m_Label->SetText(labelText);

				m_StartTicks = SDL_GetTicks();
			}
		}
	}
}

void CountdownTimer::Render()
{
	m_Label->Render();
}
