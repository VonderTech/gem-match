#include <stdlib.h>
#include <iostream>
#include <SDL.h>
#include <string>
#include "BoardModel.h"
#include <Gem.h>


BoardModel::BoardModel(const uint8_t& columns, const uint8_t& rows) :
m_Columns(columns),
m_Rows(rows),
m_Holes(0)
{
	// create array of colors
	m_Colors = new int[m_Columns * m_Rows];

	//init number generator with amout of milliseconds since SDL loaded
	m_RandomNumberGenerator.seed(SDL_GetTicks());

	// allocate enough memory for the vector that holds valid color sequences
	m_ColorSequences.reserve(m_Columns * m_Rows);

	// set valid color sequences to false
	for (int i = 0; i < m_Columns * m_Rows; ++i)
	{
		bool isValidSequence = false;
		m_ColorSequences.push_back(isValidSequence);
	}
}

BoardModel::~BoardModel()
{

	m_ColorSequences.clear();

	// delete m_Colors array
	delete[] m_Colors;


}



bool BoardModel::SwapColors(uint8_t const& sourceX, uint8_t const& sourceY, uint8_t const& targetX, uint8_t const& targetY)
{
	// check if source color is valid
	if (sourceX < 0 || sourceX > m_Columns -1 || sourceY < 0 || sourceY > m_Rows - 1 )
		return false;

	// check if target color is valid
	if (targetX < 0 || targetX > m_Columns -1  || targetY < 0 || targetY > m_Rows - 1 )
		return false;

	int sourcePosition = sourceY * m_Columns + sourceX;
	int targetPosition = targetY * m_Columns + targetX;

	// attempt to swap colors - check if the swap leads to a sequence
	uint8_t sourceColor = m_Colors[sourceY * m_Columns + sourceX];
	uint8_t targetColor = m_Colors[targetY * m_Columns + targetX];

	// swap color
	m_Colors[sourcePosition] = targetColor;
	m_Colors[targetPosition] = sourceColor;

	// check if one of them is now part of a sequence
	if (IsPartOfSequence(sourceX, sourceY) || IsPartOfSequence(targetX, targetY))
	{
		return true;
	}

	else
	{
		// reverse swap
		m_Colors[sourcePosition] = sourceColor;
		m_Colors[targetPosition] = targetColor;

		return false;
	}
}

void BoardModel::Populate()
{
	// traverse the board rows to find horizontal color sequences
	for (int y = 0; y < m_Rows; ++y)
	{
		for (int x = 0; x < m_Columns; ++x)
		{
			int position = y * m_Columns + x;

			// generate a random color for this field
			m_Colors[position] = GetRandomColor();

			// check if a horizontal sequence has been created and remove it
			if (x > 1)
			{
				if (m_Colors[position] == m_Colors[y * m_Columns + x - 1] && m_Colors[y * m_Columns + x - 2])
				{
					uint8_t newColor = m_Colors[position];

					// get a new random color that is different and does not create a sequence
					while (newColor == m_Colors[position])
					{
						newColor = GetRandomColor();
					}

					m_Colors[position] = newColor;
				}
			}

			if (y > 1)
			{
				if (m_Colors[position] == m_Colors[(y - 1) * m_Columns + x] && m_Colors[(y - 2) * m_Columns + x])
				{
					uint8_t newColor = m_Colors[position];

					// get a new random color that is different and does not create a sequence
					while (newColor == m_Colors[position] || newColor == m_Colors[(y - 1) * m_Columns + x-1])
					{
						newColor = GetRandomColor();
					}

					m_Colors[position] = newColor;
				}
			}
		}
	}

	m_Holes = 0;
}

void BoardModel::DetectColorSequences()
{
	// traverse the board rows to find horizontal color sequences
	for (int y = 0; y < m_Rows; ++y)
	{
		for (int x = 0; x < m_Columns; ++x)
		{
			int position = y * m_Columns + x;
			int color = m_Colors[position];

			// look in this row for a sequence
			
			int sequenceCountX = 1;
			int sequenceCountY = 1;

			// look horizontally - there is no need to look up the last two columns
			if (x < m_Columns - 2)
			{
				int matchingColor = color;

				while (true)
				{
					int searchPosition = position + sequenceCountX;

					// make sure to look for a sequence in the same row
					if (position / m_Rows != searchPosition / m_Rows)
						break;

					// get color of search position
					if (searchPosition < m_Columns * m_Rows)
						matchingColor = m_Colors[searchPosition];

					// stop looking if the colors don't match
					if (matchingColor != color)
						break;
					else
						++sequenceCountX;
				}
			}

			// look vertically - there is no need to look up the last two rows
			if (y < m_Rows - 2) 
			{
				int matchingColor = color;

				while (true)
				{
					int searchPosition = position + sequenceCountY * m_Columns;

					// get color of search position
					if (searchPosition < m_Columns * m_Rows)
						matchingColor = m_Colors[searchPosition];
					else
						break;
					// stop looking if the colors don't match
					if (matchingColor != color)
						break;
					else
						++sequenceCountY;
				}
			}

			// check if a sequence of at least 3 horizontal matching color has been found
			if (sequenceCountX >= 3)
			{
				for (int sequence = 0; sequence < sequenceCountX; ++sequence)
				{
					int sequencePosition = position + sequence;
					m_ColorSequences.at(sequencePosition) = true;

					// call handler
					SequenceFound(x + sequence, y);
				}
			}
			// check if a sequence of at least 3 vertical matching color has been found
			if (sequenceCountY >= 3)
			{
				for (int sequence = 0; sequence < sequenceCountY; ++sequence)
				{
					int sequencePosition = position + sequence * m_Columns;
					m_ColorSequences.at(sequencePosition) = true;

					// call handler
					SequenceFound(x, y + sequence);
				}
			}
		}
	}
}

bool const BoardModel::ContainsSequences()
{
	// iterate through color sequences
	for (std::vector<bool>::iterator it = m_ColorSequences.begin(); it != m_ColorSequences.end(); ++it)
	{
		// found the beginning of a color sequence
		if (*it == true)
			return true;
	}

	return false;
}

void BoardModel::RemoveSequences()
{
	for (int i = 0; i < m_Rows * m_Columns; ++i)
	{
		if (m_ColorSequences.at(i) == true)
		{
			m_Colors[i] = -1;

			// invoke removal event
			ColorRemoved(i % m_Columns, i / m_Rows);

			//track how many holes there are after removal
			++m_Holes;
		}
	}

	//reset valid color sequences
	for (std::vector<bool>::iterator it = m_ColorSequences.begin(); it != m_ColorSequences.end(); ++it)
	{
		*it = false;
	}
}

const uint8_t BoardModel::GetColor(const uint8_t& column, const uint8_t& row)
{
	if (row >= 0 && row < m_Rows && column >= 0 && column < m_Columns)
	{
		return m_Colors[row * m_Columns + column];
	}
	else
	{
		return 0;
	}
}

void BoardModel::PrintValidSequences()
{

	for (int i = 0; i < m_Rows * m_Columns; ++i)
	{
		if ((i % m_Columns) == 0)
			std::cout << std::endl;

		std::cout << m_ColorSequences.at(i) << " ";
	}
	// new line
	std::cout << std::endl;
}

void BoardModel::DropColors()
{
	// iterate through the board from behind and look for holes
	for (int y = m_Rows - 1; y >= 0; --y)
	{
		for (int x = m_Columns - 1; x >= 0; --x)
		{
			int position = y * m_Columns + x;

			// check if there is a hole here
			if (m_Colors[position] == -1)
			{
				// get position of color above
				int swapPosition = position - m_Columns;

				// if the position is is within the field, move the color down one cell
				if (swapPosition >= 0)
				{
					m_Colors[position] = m_Colors[swapPosition];
					m_Colors[swapPosition] = -1;

					// check if a hole has been closed
					if (m_Colors[position] != -1)
					{
						// invoke color dropped event
						ColorDropped(x, y - 1);
					}
				}
				else
				// the hole is at the top row and it gets closed with a new random color
				{
					m_Colors[position] = GetRandomColor();

					// invoke color added event
					ColorAdded(x, y);

					// one hole has bubbled up all the way to the top and has been removed
					--m_Holes;
				}

			}
		}
	}
}

void BoardModel::Print()
{
	// new line
	std::cout << std::endl;

	for (int y = 0; y < m_Rows; ++y)
	{
		for (int x = 0; x < m_Columns; ++x)
		{
			// print color ID of each cell
			std::cout << std::to_string(m_Colors[y * m_Columns + x]) << " ";
		}
		// new line
		std::cout << std::endl;
	}
}

uint8_t const BoardModel::GetRandomColor()
{
	uint8_t randomColor = uint_dist(m_RandomNumberGenerator) % (uint8_t)Gem::Color::COUNT;
	return randomColor;
}

bool BoardModel::IsPartOfSequence(uint8_t const& cellX, uint8_t const& cellY)
{
	// look in all directions and check 

	uint8_t sourceColor = m_Colors[cellY * m_Columns + cellX];

	// look right
	if (cellX < (m_Columns - 2))
	{
		if (sourceColor == m_Colors[cellY * m_Columns + cellX + 1] && sourceColor == m_Colors[cellY * m_Columns + cellX + 2])
			return true;
	}

	// look left
	if (cellX > 1)
	{
		if (sourceColor == m_Colors[cellY * m_Columns + cellX - 1] && sourceColor == m_Colors[cellY * m_Columns + cellX - 2])
			return true;
	}

	// look left and right 1 field
	if (cellX > 0 && cellX < (m_Columns - 1))
	{
		if (sourceColor == m_Colors[cellY * m_Columns + cellX - 1] && sourceColor == m_Colors[cellY * m_Columns + cellX + 1])
			return true;
	}

	// look down
	if (cellY < (m_Rows - 2))
	{
		if (sourceColor == m_Colors[(cellY + 1) * m_Columns + cellX] && sourceColor == m_Colors[(cellY + 2) * m_Columns + cellX])
			return true;
	}

	// look up
	if (cellY > 1)
	{
		if (sourceColor == m_Colors[(cellY - 1) * m_Columns + cellX] && sourceColor == m_Colors[(cellY - 2) * m_Columns + cellX])
			return true;
	}

	// look up and down 1 field
	if (cellY > 0 && cellY < (m_Rows - 1))
	{
		if (sourceColor == m_Colors[(cellY - 1) * m_Columns + cellX] && sourceColor == m_Colors[(cellY + 1) * m_Columns + cellX])
			return true;
	}

	return false;
}


void BoardModel::SetColorAddedHandler(std::function<void(const uint8_t&, const uint8_t&)> handlerFunction)
{
	ColorAdded = handlerFunction;
}

void BoardModel::SetColorRemovedHandler(std::function<void(const uint8_t&, const uint8_t&)> handlerFunction)
{
	ColorRemoved = handlerFunction;
}

void BoardModel::SetColorDroppedHandler(std::function<void(const uint8_t&, const uint8_t&)> handlerFunction)
{
	ColorDropped = handlerFunction;
}

void BoardModel::SetSequenceFoundHandler(std::function<void(const uint8_t&, const uint8_t&)> handlerFunction)
{
	SequenceFound = handlerFunction;
}

uint8_t const& BoardModel::GetColumns()
{
	return m_Columns;
}

uint8_t const& BoardModel::GetRows()
{
	return m_Rows;
}

uint8_t const& BoardModel::GetHoles()
{
	return m_Holes;
}
