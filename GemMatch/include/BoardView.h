#ifndef __BOARDVIEW_H__
#define __BOARDVIEW_H__

#include <functional>
#include "IMouseListener.h"
#include "Gem.h"

class Gem;
class GemFactory;

/*! Displays the state of the BoardModel to the user and processes user input
*	
*/
class BoardView : IMouseListener
{
public:
	BoardView(const uint8_t& columns, const uint8_t& rows);
	virtual ~BoardView();

	enum class State { Freeze, SelectFirstGem, SelectSecondGem, RemoveGems };

	/*! \brief Sets the position of the board
	*         
	*	\param x The x position
	*	\param y The y postion
	*/
	void SetPosition(const int& x, const int& y);

	/*! \brief Sets the width of the board
	*
	*	\param width The desired width
	*/
	void SetWidth(const int& width);

	/*! \brief Sets the height of the board
	*
	*	\param height The desired height
	*/
	void SetHeight(const int& height);

	/*! \brief Retrieves the gem of the specified cell
	*
	*	\param column The column of the requested cell
	*	\param row The row of the requested cell
	*/
	Gem* GetGem(const uint32_t& column, const uint32_t& row) const;

	/*! \brief Adds a gem to a cell
	*
	*	\param column The column of the requested cell
	*	\param row The row of the requested cell
	*	\param color The color of the gem to be added
	*/
	void AddGem(const uint32_t& column, const uint32_t& row, const Gem::Color& color);

	/*! \brief Marks a gems that will be removed
	*
	*	\param targetColumn Horizontal postion of target cell
	*	\param targetRow Vertical postion of target cell
	*/
	void MarkRemovedGem(const uint32_t& column, const uint32_t& row);

	/*! \brief Removes a gems
	*
	*	\param targetColumn Horizontal postion of target cell
	*	\param targetRow Vertical postion of target cell
	*/
	void RemovedGem(const uint32_t& column, const uint32_t& row);

	/*! \brief Moves a gem from the source cell to the target cell
	*
	*	\param gem Pointer to the gem to be moved
	*	\param targetColumn Horizontal postion of target cell
	*	\param targetRow Vertical postion of target cell
	*/
	void MoveGem(Gem* gem, const uint8_t& targetColumn, const uint8_t& targetRow);

	/*! \brief Renders the board view
	*
	*/
	void Render();

	/*! \brief Enables or disables user input
	*
	*	\param isInputEnabled Flag to enable or disable input
	*/
	void EnableInput(const bool& isInputEnabled);

	/*! \brief Checks if the board view is accepting user input
	*
	*	\return true if enabled and false if disabled
	*/
	const bool IsInputEnabled();

	// registration method for handlers that want to know which gems have been selected
	void SetGemsSelectedHandler(std::function<void(const uint8_t&, const uint8_t&, const uint8_t&, const uint8_t&)> handlerFunction);

private:
	// process mouse events
	void OnMouseMove(const uint32_t& in_MouseX, const uint32_t& in_MouseY) override;
	void OnMouseDown(SDL_Event* in_Event) override;
	void OnMouseReleased(SDL_Event* in_Event) override;
	
	// highlight a cell
	void HighlightCell(const uint8_t& column, const uint8_t& row);

	// resets the state of a gem
	void ResetCell(const uint8_t& column, const uint8_t& row);

	void SwapCells(const int& firstCell, const int& secondCell);

	// detects if mouse is within board bounds
	bool IsMouseOverBoard();

	// amount of cells
	const uint8_t m_Columns;
	const uint8_t m_Rows;

	// position and dimensions of the board
	uint32_t m_PosX;
	uint32_t m_PosY;
	uint32_t m_Width;
	uint32_t m_Height;

	// dimensions of the cells
	uint32_t m_CellWidth;
	uint32_t m_CellHeight;

	// mouse position
	uint32_t m_MousePosX;
	uint32_t m_MousePosY;

	// tracks if user presses the left mouse button
	bool m_IsLeftMouseButtonDown;

	// array that holds all gems
	Gem** m_Gems;

	// the factory that produces gems
	GemFactory* m_GemFactory;

	// holds the current board view state
	State m_State;

	// track highlighted cell
	uint32_t m_HighlightedCell;

	// track selection
	int m_FirstCell;
	int m_SecondCell;

	Gem* m_HoveredGem = nullptr;

	// a cursor that indicates the selected gem
	Sprite* m_SelectionCursor = nullptr;
	// sprite for highlighting removed gems
	Sprite* m_RemoveHighlight = nullptr;

	// handler functions
	std::function <void(const uint8_t&, const uint8_t&, const uint8_t&, const uint8_t&)> GemsSelected;

	// file paths to sprites
	static const std::string SELECTION_CURSOR_PATH;
	static const std::string REMOVE_SPRITE_PATH;
};

#endif
