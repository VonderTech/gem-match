#ifndef __BOARDMODEL_H__
#define __BOARDMODEL_H__

#include <random>
#include <cstdint>
#include <vector>
#include <functional>

/*! The underlying representation of the game board.
*	Notifies the BoardController when there has been a change in its state
*/
class BoardModel
{
public:
	BoardModel(const uint8_t& columns, const uint8_t& rows);
	virtual ~BoardModel();

	/*! \brief Retrieves the color of a cell
	*
	*	\param column The column of the requested cell
	*	\param row The row of the requested cell 
	*/
	const uint8_t GetColor(const uint8_t& column, const uint8_t& row);

	/*! \brief Swaps the color of the source cell with the color of the target cell
	*
	*	\param sourceX Horizontal postion of source cell
	*	\param sourceY Vertical postion of source cell
	*	\param targetX Horizontal postion of target cell
	*	\param targetY Vertical postion of target cell
	*/
	bool SwapColors(const uint8_t& sourceX, const uint8_t& sourceY, const uint8_t& targetX, const uint8_t& targetY);

	/*! \brief Populates the board with random colors and makes sure that the board does not contain any sequences
	*
	*/
	void Populate();

	/*! \brief Detects matching color sequences on the board
	*
	*/
	void DetectColorSequences();

	/*! \brief Detect whether the current board state contains any matching color sequences or not
	*
	*/
	const bool ContainsSequences();

	/*! \brief Removes matching color sequences from the board
	*
	*/
	void RemoveSequences();

	/*! \brief Drops colors into holes that are beneath
	*
	*	Holes rise like bubbles to the top. When they arrive at the top they get filled with a new random color
	*/
	void DropColors();

	/*! \brief Prints matching color sequences to the consoles
	*
	*/
	void PrintValidSequences();

	/*! \brief Prints the state of the board to the console
	*
	*/
	void Print();

	// returns the amount of columns
	const uint8_t& GetColumns();
	// returns the amount of rows
	const uint8_t& GetRows();
	// returns amount of holes
	const uint8_t& GetHoles();

	// registration methods for handlers that are interested in state changes
	void SetColorAddedHandler(std::function<void(const uint8_t&, const uint8_t&)> handlerFunction);
	void SetColorRemovedHandler(std::function<void(const uint8_t&, const uint8_t&)> handlerFunction);
	void SetColorDroppedHandler(std::function<void(const uint8_t&, const uint8_t&)> handlerFunction);
	void SetSequenceFoundHandler(std::function<void(const uint8_t&, const uint8_t&)> handlerFunction);

private:
	// grid size
	const uint8_t m_Columns;
	const uint8_t m_Rows;

	// array that holds the color information of each board cell
	int* m_Colors;

	// amount of holes in board
	uint8_t m_Holes;

	const uint8_t GetRandomColor();

	/*! \brief Checks if the color of the cell is part of a sequence
	*/
	bool IsPartOfSequence(const uint8_t& cellX, const uint8_t& cellY);

	// a bit array that describes valid color sequences on the board
	std::vector<bool> m_ColorSequences;

	// the Mersenne Twister Engine for random number generation
	typedef std::mt19937 MersenneNumberGenerator;
	MersenneNumberGenerator m_RandomNumberGenerator;

	// get a uniform distribution for random number generation
	std::uniform_int_distribution<uint32_t> uint_dist;

	// handler functions
	std::function <void(const uint8_t&, const uint8_t&)> ColorAdded;
	std::function <void(const uint8_t&, const uint8_t&)> ColorRemoved;
	std::function <void(const uint8_t&, const uint8_t&)> ColorDropped;
	std::function <void(const uint8_t&, const uint8_t&)> SequenceFound;
};
#endif
