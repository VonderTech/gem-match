#ifndef __LOG_H__
#define __LOG_H__

#include <string>

/*! Provides logging functionality to write messages to the console
*
*/
class Log
{
private:
	// Singleton
	static Log Instance;

private:
	Log();
	virtual ~Log();

public:

	/*! \brief Logs an SDL error
	*
	*	\param message The error message to write, along with the SDL_GetError()
	*/
	void LogSDLError(const std::string &message);

	/*! \brief Print a message to the console
	*
	*	\param message The error message to write
	*/
	void LogMessage(const std::string message);

	// returns singleton instance
	static Log* GetInstance();
};
#endif
