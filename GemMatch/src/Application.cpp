#include <functional>
#include <SDL_image.h>
#include "Log.h"
#include "Sprite.h"
#include "Label.h"
#include "GemFactory.h"
#include "InputManager.h"
#include "BoardModel.h"
#include "BoardView.h"
#include "BoardController.h"
#include "CountdownTimer.h"

#include "Application.h"

const std::string Application::FONT_PATH = "assets/fonts/Big_Bottom_Typeface_Normal.ttf";
const std::string Application::BACKGROUND_PATH = "assets/sprites/background.jpg";
const std::string Application::OVERLAY_PATH = "assets/sprites/overlay.png";

// ensure there is only a single instance of the class.
Application Application::Instance;

bool Application::Initialize()
{
	// logging
	Log* logger = Log::GetInstance();

	// manager for handling input
	m_InputMgr = InputManager::GetInstance();

	// init SDL with all subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		logger->LogSDLError("Failed to initalize SDL.");
		return false;
	}

	// create a window
	m_Window = SDL_CreateWindow("Gem Match", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_WindowWidth, m_WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (m_Window == nullptr)
	{
		logger->LogSDLError("Failed to create window");
		return false;
	}

	// create a renderer
	m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	if (m_Renderer == nullptr)
	{
		logger->LogSDLError("Failed to create renderer");
		return false;
	}

	// initialize PNG loader
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
		logger->LogSDLError("Failed to initialize the PNG loader");
		return 0;
	}

	// initialize True type font support
	if (TTF_Init() == -1){
		logger->LogSDLError("Failed to initialize the TFF loader");
		return 1;
	}

	// load background image
	m_Background = new Sprite();
	m_Background->Load(BACKGROUND_PATH);

	// create the board controller
	m_BoardController = new BoardController();
	// init to fill board with gems
	m_BoardController->Initialize();
	m_BoardController->Freeze(true);

	// create and setup a countdown timer
	m_CountdownTimer = new CountdownTimer(60);
	m_CountdownTimer->SetInterval(1000);
	// register callback
	m_CountdownTimer->SetFinishedHandler(std::bind(&Application::OnCountdownFinished, this));

	// setup overlay for start and game over screen
	m_OverlayBackground = new Sprite();
	m_OverlayBackground->Load(OVERLAY_PATH);
	m_OverlayBackground->SetWidth(m_WindowWidth);
	m_OverlayBackground->SetHeight(m_WindowHeight);

	// labels for start screen
	m_WelcomeLabel = new Label(40);
	m_WelcomeLabel->SetText("Welcome to Gem Match");
	m_WelcomeLabel->SetColor(0, 60, 80);
	m_WelcomeLabel->SetPosition(50, 100);

	m_StartGameLabel = new Label(30);
	m_StartGameLabel->SetText("Press <Space> to start");
	m_StartGameLabel->SetColor(10, 120, 10);
	m_StartGameLabel->SetPosition(100, 400);

	//labels for game over screen
	m_GameOverLabel = new Label(70);
	m_GameOverLabel->SetText("Game Over");
	m_GameOverLabel->SetColor(120, 0, 0);
	m_GameOverLabel->SetPosition(100, 100);

	m_PlayAgainLabel = new Label(30);
	m_PlayAgainLabel->SetText("Press <Space> to play again");
	m_PlayAgainLabel->SetColor(10, 120, 10);
	m_PlayAgainLabel->SetPosition(100, 400);

	// game has not started yet
	m_HasGameStarted = false;
	// game has not finished yet
	m_HasGameFinished = false;
	// game is now up and running
	m_IsRunning = true;

	return true;
}

void Application::CleanUp()
{
	delete m_PlayAgainLabel;
	delete m_GameOverLabel;
	delete m_StartGameLabel;
	delete m_WelcomeLabel;
	delete m_OverlayBackground;
	delete m_CountdownTimer;
	delete m_Background;
	delete m_BoardController;
	SDL_DestroyRenderer(m_Renderer);
	SDL_DestroyWindow(m_Window);

	//Quit SDL_ttf
	TTF_Quit();

	SDL_Quit();
}

void Application::Update()
{
	m_BoardController->Update();
	m_CountdownTimer->Update();
}

void Application::Render()
{
	SDL_RenderClear(m_Renderer);
	
	m_Background->Render();
	m_BoardController->GetView()->Render();
	m_CountdownTimer->Render();

	// show a welcome screen, if the game is up for the first time
	if (m_HasGameStarted == false)
	{
		m_OverlayBackground->Render();
		m_WelcomeLabel->Render();
		m_StartGameLabel->Render();
	}

	// show game over screen when game is finished
	if (m_HasGameFinished == true)
	{
		m_OverlayBackground->Render();
		m_GameOverLabel->Render();
		m_PlayAgainLabel->Render();
	}
	
	// render everything to screen
	SDL_RenderPresent(m_Renderer);
}

void Application::OnEvent(SDL_Event* in_Event)
{
	if (in_Event->type == SDL_QUIT)
	{
		m_IsRunning = false;
	}

	if (in_Event->type == SDL_MOUSEMOTION)
	{
		Log::GetInstance()->LogMessage("Mouse motion");
	}
}

void Application::OnCountdownFinished()
{
	m_HasGameFinished = true;
	// freeze board
	m_BoardController->Freeze(true);
	m_CountdownTimer->Reset();
}

int Application::Execute(int argc, char* argv[])
{
	// try to initialize the app
	if (Initialize() != true)
	{
		return 0;
	}
	
	SDL_Event event;

	// main loop
	while (m_IsRunning)
	{
		// poll sdl events
		while (SDL_PollEvent(&event) != 0)
		{
			// check if user closes application
			if (event.type == SDL_QUIT)
			{
				m_IsRunning = false;
			}

			// check if user pressed the space bar
			if (event.type == SDL_KEYDOWN)
			{
				// game starts for the first time
				if (m_HasGameStarted == false && event.key.keysym.sym == SDLK_SPACE)
				{
					// unfreeze board
					m_BoardController->Freeze(false);
					// start countdown
					m_CountdownTimer->Start();

					m_HasGameStarted = true;
				}
				// game has not started yet
				if (m_HasGameFinished == true && event.key.keysym.sym == SDLK_SPACE)
				{
					// init again to populate board with new gems
					m_BoardController->Initialize();

					// unfreeze board
					m_BoardController->Freeze(false);

					// reset & start countdown
					m_CountdownTimer->Reset();
					m_CountdownTimer->Start();

					m_HasGameFinished = false;
				}
			}

			// send all events to the input manager
			m_InputMgr->OnEvent(&event);
		}

		// update logic
		Update();

		// render
		Render();

		// maintain target frame rate
		m_DelayTime = m_WaitTime - (SDL_GetTicks() - m_FrameStartTime);
		if (m_DelayTime > 0)
		{
			SDL_Delay(m_DelayTime);
		}
		m_FrameStartTime = SDL_GetTicks();
	}

	// free resources
	CleanUp();

	return 1;
}

SDL_Renderer* Application::GetRenderer() const
{
	return m_Renderer;
}

std::string Application::GetMainFontPath() const
{
	return FONT_PATH;
}

Application* Application::GetInstance()
{
	return &Application::Instance;
}
