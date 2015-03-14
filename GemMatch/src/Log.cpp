#include <iostream>
#include <string>
#include <SDL.h>
#include "Log.h"

Log Log::Instance;

Log::Log()
{
}

Log::~Log()
{
}

void Log::LogSDLError(const std::string &message)
{
	std::cout << message << " error: " << SDL_GetError() << std::endl;
}

void Log::LogMessage(const std::string message)
{
	std::cout << message << std::endl;
}

Log* Log::GetInstance()
{
	return &Log::Instance;
}
