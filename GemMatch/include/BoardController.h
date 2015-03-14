#ifndef __BOARD_CONTROLLER_H__
#define __BOARD_CONTROLLER_H__

class BoardModel;
class BoardView;

/*! Sits in the middle of the BoardModel and the BoardView and manages both.
*	Sets up the board and handles the game logic.
*/
class BoardController
{
public:
	BoardController();
	virtual ~BoardController();

	/*! \brief The various states of the board
	*
	*/
	enum class BoardState { Uninitialized, Freeze, Ready, GemsSwapped, GemsSelected, SequenceDetected, SequencesRemoved, GemsDopped };

	/*! \brief Initialized the board with a fresh set of gems
	*
	*/
	void Initialize();

	/*! \brief Freezes board: logic is no longer processed and user input gets disables
	*
	*/
	void Freeze(const bool& isFreezed);

	/*! \brief Processes the game logic
	*
	*/
	void Update();

	/*! Returns a pointer to the board view	*/
	BoardView* GetView() const { return m_View; }

private:
	/*! Swaps selected gems	*/
	void SwapGems();

	/*! Detects color sequences in the board */
	void FindSequences();

	/*! Removes color sequences from the board */
	void RemoveSequences();

	/*! Drops gems one row at a time */
	void DropGems();

	/*! Callback handler that gets invoked when two gems have been selected */
	void OnGemsSelected(const uint8_t& sourceX, const uint8_t& sourceY, const uint8_t& targetX, const uint8_t& targetY);

	/*! Callback handler that gets invoked when a new color has been added to the board */
	void OnColorAdded(const uint8_t& column, const uint8_t& row);

	/*! Handler that gets invoked when a color has been removed to the board */
	void OnColorRemoved(const uint8_t& column, const uint8_t& row);

	/*! Handler that gets invoked when a color dropped down one row */
	void OnColorDropped(const uint8_t& column, const uint8_t& row);

	/*! Handler that gets invoked when a color is detected to be part of a sequence */
	void OnSequenceFound(const uint8_t& column, const uint8_t& row);

private:

	BoardState m_BoardState;
	BoardModel* m_Board;
	BoardView* m_View;

	// amount of delay between dropping one row of gems in milliseconds
	uint32_t m_DropDelay;
	uint32_t m_DropStartTime;

	// amount of delay after removing a sequence of gems in milliseconds
	uint32_t m_RemoveSequenceDelay;
	uint32_t m_RemoveSequenceStartTime;

	// store selected gems
	uint8_t m_SelectedSourceCellX;
	uint8_t m_SelectedSourceCellY;
	uint8_t m_SelectedTargetCellX;
	uint8_t m_SelectedTargetCellY;


};

#endif

