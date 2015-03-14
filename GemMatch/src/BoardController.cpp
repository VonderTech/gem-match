#include <iostream>
#include "BoardModel.h"
#include "BoardView.h"

#include "BoardController.h"

BoardController::BoardController() :
	m_BoardState(BoardState::Uninitialized),
	m_Board(new BoardModel(8,8)),
	m_View(new BoardView(8,8)),
	m_DropDelay(100),
	m_DropStartTime(0),
	m_RemoveSequenceDelay(250),
	m_RemoveSequenceStartTime(0)
{
	// set view properties
	m_View->SetPosition(330, 110);
	m_View->SetWidth(340);
	m_View->SetHeight(340);
	
	// register callbacks
	m_View->SetGemsSelectedHandler(
		std::bind(&BoardController::OnGemsSelected, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	m_Board->SetColorAddedHandler(std::bind(&BoardController::OnColorAdded, this, std::placeholders::_1, std::placeholders::_2));
	m_Board->SetColorRemovedHandler(std::bind(&BoardController::OnColorRemoved, this, std::placeholders::_1, std::placeholders::_2));
	m_Board->SetColorDroppedHandler(std::bind(&BoardController::OnColorDropped, this, std::placeholders::_1, std::placeholders::_2));
	m_Board->SetSequenceFoundHandler(std::bind(&BoardController::OnSequenceFound, this, std::placeholders::_1, std::placeholders::_2));
}


BoardController::~BoardController()
{

	delete m_View;
	delete m_Board;
}

void BoardController::Initialize()
{
	// populate the board
	m_Board->Populate();

	// board dimensions
	uint8_t columns = m_Board->GetColumns();
	uint8_t rows = m_Board->GetRows();

	// add a gem representation to the view 
	for (int y = 0; y < rows; ++y)
	{
		for (int x = 0; x < columns; ++x)
		{
			uint8_t color = m_Board->GetColor(x, y);
			m_View->AddGem(x, y, Gem::Color(color));
		}
	}

	// controller is ready for playing
	m_BoardState = BoardState::Ready;
}

void BoardController::Freeze(bool const& isFreezed)
{
	if (isFreezed == true)
		// controller is now freezed
		m_BoardState = BoardState::Freeze;
	else
		// enable play
		m_BoardState = BoardState::Ready;
}

void BoardController::Update()
{
	switch (m_BoardState)
	{
	case BoardState::Freeze:
		// make sure view is not accepting user input
		if (m_View->IsInputEnabled() == true)
			m_View->EnableInput(false);
		break;
	case BoardState::Ready:
		// make sure view is accepting input
		if (m_View->IsInputEnabled() == false)
			m_View->EnableInput(true);
		break;
	case BoardState::GemsSelected:
		// try to swap selected gems
		SwapGems();
		break;
	case BoardState::GemsSwapped:
		// find sequences
		FindSequences();
		break;
	case BoardState::SequenceDetected:
		// remove all sequences
		RemoveSequences();
		break;
	case BoardState::SequencesRemoved:
		//drop gems one row at a time until no more holes are left
		DropGems();
		break;
	case BoardState::GemsDopped:
		// find sequences
		FindSequences();
		break;
	default:
		break;
	}
	/* in every update cycle:
	1. Check if gems have been swapped 
		yes -> 
		- detect and remove sequences
	2. Check if sequences have been removed

	*/
}

void BoardController::OnGemsSelected(const uint8_t& sourceX, const uint8_t& sourceY, const uint8_t& targetX, const uint8_t& targetY)
{
	// store coordinates of source and target cell
	m_SelectedSourceCellX = sourceX;
	m_SelectedSourceCellY = sourceY;
	m_SelectedTargetCellX = targetX;
	m_SelectedTargetCellY = targetY;

	// gem have been selected
	m_BoardState = BoardState::GemsSelected;
}

void BoardController::SwapGems()
{
	// check if the attempted swap leads to a sequence
	bool isSwapValid = m_Board->SwapColors(m_SelectedSourceCellX, m_SelectedSourceCellY, m_SelectedTargetCellX, m_SelectedTargetCellY);

	// get the corresponding gems
	Gem* firstGem = m_View->GetGem(m_SelectedSourceCellX, m_SelectedSourceCellY);
	Gem* secondGem = m_View->GetGem(m_SelectedTargetCellX, m_SelectedTargetCellY);

	if (isSwapValid)
	{
		// swap gems in view
		m_View->MoveGem(firstGem, m_SelectedTargetCellX, m_SelectedTargetCellY);
		m_View->MoveGem(secondGem, m_SelectedSourceCellX, m_SelectedSourceCellY);

		// gems have been swapped successfully
		m_BoardState = BoardState::GemsSwapped;
	}
	else
	{
		// move was invalid and board is ready for a new selection
		m_BoardState = BoardState::Ready;
	}
}

void BoardController::FindSequences()
{
	// find matching color sequences
	m_Board->DetectColorSequences();

	if (m_Board->ContainsSequences() == true)
		m_BoardState = BoardState::SequenceDetected;
	else
		m_BoardState = BoardState::Ready;
}

void BoardController::RemoveSequences()
{
	// add some delay before gems are removed:
	// the user get a better understanding of the action as removed gems get highlighted in the view 
	if (m_RemoveSequenceStartTime == 0)
	{
		m_RemoveSequenceStartTime = SDL_GetTicks();
	}
	else
	{
		int currentTime = SDL_GetTicks();

		// if delay is over, indicate that all sequences have been removed
		if (currentTime - m_RemoveSequenceDelay > m_RemoveSequenceStartTime)
		{
			// remove all sequences
			m_Board->RemoveSequences();

			m_BoardState = BoardState::SequencesRemoved;

			// reset start time
			m_RemoveSequenceStartTime = 0;
		}
	}
}

void BoardController::DropGems()
{
	// as long as there are any holes in the board, keep dropping colors
	if (m_Board->GetHoles() > 0)
	{
		// add some delay each time gems are dropped
		if (m_DropStartTime == 0)
		{
			m_DropStartTime = SDL_GetTicks();
		}
		else
		{
			int currentTime = SDL_GetTicks();
			if (currentTime - m_DropDelay > m_DropStartTime)
			{
				m_Board->DropColors();

				// reset start time
				m_DropStartTime = 0;
			}
		}
		
	}
	else
	{
		m_BoardState = BoardState::GemsDopped;
	}	
}

void BoardController::OnColorAdded(const uint8_t& Column, const uint8_t& Row)
{
	// retrieve color
	uint8_t color = m_Board->GetColor(Column, Row);

	// add a new gem to the view
	m_View->AddGem(Column, Row, Gem::Color(color));
}

void BoardController::OnColorRemoved(uint8_t const& column, uint8_t const& row)
{
	// gems that are going to be removed get hightlighted
	m_View->RemovedGem(column, row);
}


void BoardController::OnColorDropped(const uint8_t& column, const uint8_t& row)
{
	//retrieve gem at position
	Gem* gem = m_View->GetGem(column, row);
	// move down one row
	m_View->MoveGem(gem, column, row + 1);
}

void BoardController::OnSequenceFound(const uint8_t& column, const uint8_t& row)
{
	m_View->MarkRemovedGem(column, row);
}