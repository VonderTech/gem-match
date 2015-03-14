//#ifdef _DEBUG
//	// visual leak detector
//	#include <vld.h>
//#endif

#include <SDL.h>
#include "Application.h"

int main(int argc, char* argv[])
{
	// get main application instance & start it
	Application* app = Application::GetInstance();
	return app->Execute(argc, argv);
}
