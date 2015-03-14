#include <sstream>
#include <cmath>  
#include <string>
#include <InputManager.h>
#include "IMouseListener.h"
#include "GemFactory.h"
#include "BoardView.h"

const std::string BoardView::SELECTION_CURSOR_PATH = "assets/sprites/cursor.png";
const std::string BoardView::REMOVE_SPRITE_PATH = "assets/sprites/highlight.png";

BoardView::BoardView(const uint8_t& columns, const uint8_t& rows) :
	m_Columns(columns),
	m_Rows(rows),
	m_PosX(0),
	m_PosY(0),
	m_IsLeftMouseButtonDown(false),
	m_Width(100),
	m_Height(100),
	m_CellWidth(0),
	m_CellHeight(0),
	m_MousePosX(0),
	m_MousePosY(0),
	m_GemFactory(new GemFactory()),
	m_SelectionCursor(new Sprite),
	m_RemoveHighlight(new Sprite),
	m_State(State::SelectFirstGem)
{
	// listen for mouse events
	InputManager::GetInstance()->AddMouseListener(this);

	// create array of gems
	m_Gems = new Gem*[m_Columns * m_Rows];

	// calculate cell dimensions
	m_CellWidth = m_Width / m_Columns;
	m_CellHeight = m_Height / m_Rows;
	m_SelectionCursor->Load(SELECTION_CURSOR_PATH);
	m_RemoveHighlight->Load(REMOVE_SPRITE_PATH);
}


BoardView::~BoardView()
{
	// listen for mouse events
	InputManager::GetInstance()->RemoveMouseListener(this);
	// delete gem array
	delete[] m_Gems;
	delete m_RemoveHighlight;
	delete m_SelectionCursor;
	delete m_GemFactory;
}


void BoardView::SetPosition(const int& x, const int& y)
{
	m_PosX = x;
	m_PosY = y;
}

void BoardView::SetWidth(const int& width)
{
	m_Width = width;

	// update cell width
	m_CellWidth = m_Width / m_Columns;
}

void BoardView::SetHeight(const int& height)
{
	m_Height = height;

	// update cell height
	m_CellHeight = m_Height / m_Rows;
}

Gem* BoardView::GetGem(uint32_t const& column, uint32_t const& row) const
{
	if (row >= 0 && row < m_Rows && column >= 0 && column < m_Columns)
	{
		return m_Gems[row * m_Columns + column];
	}

	return nullptr;
}

void BoardView::AddGem(const uint32_t& column, const uint32_t& row, const Gem::Color& color)
{
	if (row >= 0 && row < m_Rows && column >= 0 && column < m_Columns)
	{
		// get a gem from the factory
		m_Gems[row * m_Columns + column] = m_GemFactory->GetGem(color);
	}
}

void BoardView::MarkRemovedGem(const uint32_t& column, const uint32_t& row)
{
	if (row >= 0 && row < m_Rows && column >= 0 && column < m_Columns)
	{
		Gem* gem = m_Gems[row * m_Columns + column];
		gem->SetState(Gem::State::Removed);
	}
}

void BoardView::RemovedGem(const uint32_t& column, const uint32_t& row)
{
	if (row >= 0 && row < m_Rows && column >= 0 && column < m_Columns)
	{
		Gem* gem = m_Gems[row * m_Columns + column];

		// gem is no longer used and can be recycled and reused by the gem factory
		if (gem != nullptr)
			gem->SetUsed(false);

		// remove gem
		m_Gems[row * m_Columns + column] = nullptr;
	}
}

void BoardView::MoveGem(Gem* gem, const uint8_t& targetColumn, const uint8_t& targetRow)
{
	// cell exists, move the gem there
	if (targetRow >= 0 && targetRow < m_Rows && targetColumn >= 0 && targetColumn < m_Columns)
	{
		m_Gems[targetRow * m_Columns + targetColumn] = gem;

	}
}

void BoardView::EnableInput(const bool& isInputEnabled)
{
	if (isInputEnabled == false)
		m_State = State::Freeze;
	else
		m_State = State::SelectFirstGem;
}

bool const BoardView::IsInputEnabled()
{
	if (m_State == State::Freeze)
		return false;
	else
		return true;
}

void BoardView::SetGemsSelectedHandler(std::function<void(const uint8_t&, const uint8_t&, const uint8_t&, const uint8_t&)> handlerFunction)
{
	GemsSelected = handlerFunction;
}

void BoardView::OnMouseMove(const uint32_t& mouseX, const uint32_t& mouseY)
{
	// do nothing if input is disabled
	if (m_State == State::Freeze)
		return;

	m_MousePosX = mouseX;
	m_MousePosY = mouseY;

	// check if mouse is placed over board
	if (IsMouseOverBoard() == true)
	{
		// determine the cell the mouse is hovering
		uint8_t xCell = (m_MousePosX - m_PosX) / m_CellWidth;
		uint8_t yCell = (m_MousePosY - m_PosY) / m_CellHeight;

		if (xCell > m_Columns - 1) xCell = m_Columns - 1;
		if (yCell > m_Rows - 1) yCell = m_Rows - 1;

		//highlight new cell if mouse hovers over a different cell
		uint8_t hoveredCell = yCell * m_Columns + xCell;

		if (m_State == State::SelectFirstGem)
		{
			Gem* highlightedGem = m_Gems[m_HighlightedCell];

			// if there is a gem at the position, highlight it
			if (highlightedGem != nullptr)
			{
				if (hoveredCell != m_HighlightedCell || highlightedGem->GetState() == Gem::State::Normal)
				{
					ResetCell(m_HighlightedCell % m_Columns, m_HighlightedCell / m_Rows);
					HighlightCell(xCell, yCell);
				}
			}
			m_HighlightedCell = hoveredCell;
		}
	}
	// remove hightlighting when outside of board
	else
	{
		Gem* highlightedGem = m_Gems[m_HighlightedCell];

		// if there is a gem at the position, highlight it
		if (highlightedGem != nullptr)
		{
			if (highlightedGem->GetState() == Gem::State::Highlighted)
			{
				// switch off highlighting if the mouse is not over the board
				ResetCell(m_HighlightedCell % m_Columns, m_HighlightedCell / m_Rows);
			}
		}
	}
}

void BoardView::OnMouseDown(SDL_Event* event)
{
	// do nothing if input is disabled
	if (m_State == State::Freeze)
		return;

	// check if mouse is placed over board
	if (event->button.button == SDL_BUTTON_LEFT)
	{
		m_IsLeftMouseButtonDown = true;

		// determine the cell where the user clicked
		uint8_t xCell = (m_MousePosX - m_PosX) / m_CellWidth;
		uint8_t yCell = (m_MousePosY - m_PosY) / m_CellHeight;

		if (xCell > m_Columns - 1) xCell = m_Columns - 1;
		if (yCell > m_Rows - 1) yCell = m_Rows - 1;

		if (m_State == State::SelectFirstGem)
		{
			m_FirstCell = yCell * m_Columns + xCell;

			m_Gems[m_FirstCell]->SetState(Gem::State::Selected);
			m_State = State::SelectSecondGem;
		}
	}
}

void BoardView::OnMouseReleased(SDL_Event* event)
{
	// do nothing if input is disabled
	if (m_State == State::Freeze)
		return;

	if (event->button.button == SDL_BUTTON_LEFT)
	{
		m_IsLeftMouseButtonDown = false;

		// check if mouse button was released inside the board
		if (IsMouseOverBoard() == true)
		{
			// determine the cell where the mouse button was released
			uint8_t xCell = (m_MousePosX - m_PosX) / m_CellWidth;
			uint8_t yCell = (m_MousePosY - m_PosY) / m_CellHeight;

			if (xCell > m_Columns - 1) xCell = m_Columns - 1;
			if (yCell > m_Rows - 1) yCell = m_Rows - 1;

			// store cell where mouse button was released
			int releasedCell = yCell * m_Columns + xCell;

			// user selected the second gem
			if (m_State == State::SelectSecondGem && m_FirstCell != releasedCell)
			{
				// invoke gems selected event
				//GemsSelected(m_FirstCell % m_Columns, m_FirstCell / m_Rows, releasedCell % m_Columns, releasedCell / m_Rows);
				SwapCells(m_FirstCell, releasedCell);

				m_Gems[m_FirstCell]->SetState(Gem::State::Normal);
				m_State = State::SelectFirstGem;
				
			}

			// user performed a click and release the button over the same cell
			if (m_State == State::SelectSecondGem && m_FirstCell == releasedCell)
			{
				m_Gems[releasedCell]->SetState(Gem::State::Selected);
			}
		}
	}
}

void BoardView::HighlightCell(const uint8_t& column, const uint8_t& row)
{
	uint32_t cellID = row * m_Columns + column;

	if (m_Gems[cellID] != nullptr)
	{
		m_Gems[cellID]->SetState(Gem::State::Highlighted);
	}
}

void BoardView::ResetCell(const uint8_t& column, const uint8_t& row)
{
	uint32_t cellID = row * m_Columns + column;

	if (m_Gems[cellID] != nullptr)
	{
		m_Gems[cellID]->SetState(Gem::State::Normal);
	}
}

void BoardView::SwapCells(const int& firstCell, const int& secondCell)
{

	// swap if cells are adjacent
	if (((firstCell / m_Rows) == (secondCell / m_Rows) && abs(firstCell - secondCell) == 1)
		|| abs(firstCell - secondCell) == m_Columns)
	{
		// invoke gems selected event
		GemsSelected(m_FirstCell % m_Columns, m_FirstCell / m_Rows, secondCell % m_Columns, secondCell / m_Rows);

	}
}


bool BoardView::IsMouseOverBoard()
{
	return (m_MousePosX > m_PosX && m_MousePosX < m_PosX + m_Width
		&& m_MousePosY > m_PosY && m_MousePosY < m_PosY + m_Height);
}

void BoardView::Render()
{

	//Draw the tiles by calculating their positions
	for (int i = 0; i < m_Rows * m_Columns; ++i)
	{
		int x = i % m_Columns;
		int y = i / m_Rows;

		Gem* gem = m_Gems[i];
		if (gem != nullptr)
		{
			Sprite* currentSprite = gem->GetSprite();

			// draw normal state
			if (gem->GetState() == Gem::State::Normal || gem->GetState() == Gem::State::Highlighted)
			{
				currentSprite->SetPosition(m_PosX + x * m_CellWidth, m_PosY + y * m_CellHeight);
				gem->Render();
			}

			// draw remove highlight
			if (gem->GetState() == Gem::State::Removed)
			{
				currentSprite->SetPosition(m_PosX + x * m_CellWidth, m_PosY + y * m_CellHeight);
				m_RemoveHighlight->SetPosition(m_PosX + x * m_CellWidth, m_PosY + y * m_CellHeight);
				gem->Render();
				m_RemoveHighlight->Render();
			}
		}
	}

	if (m_State == State::SelectSecondGem)
	{
		Sprite* selectedSprite = m_Gems[m_FirstCell]->GetSprite();

		int x = m_FirstCell % m_Columns;
		int y = m_FirstCell / m_Rows;

		if (m_IsLeftMouseButtonDown)
		{
			SDL_Rect rect = selectedSprite->GetRect();
			selectedSprite->SetPosition(m_MousePosX - rect.w/2, m_MousePosY - rect.h/2);
			m_Gems[m_FirstCell]->Render();
		}
		else
		{
			selectedSprite->SetPosition(m_PosX + x * m_CellWidth, m_PosY + y * m_CellHeight);
			m_SelectionCursor->SetPosition(m_PosX + x * m_CellWidth, m_PosY + y * m_CellHeight);

			// render gem & the selection cursor
			m_Gems[m_FirstCell]->Render();
			m_SelectionCursor->Render();
		}
	}
}
