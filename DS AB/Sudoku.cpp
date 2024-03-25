/*!*************************************************************************
\file Sudoku.cpp
\author Seetoh Wei Tung
\par DP email: seetoh.w@digipen.edu
\par Course: Data Structures
\par Assignment Bonus
\date 24-03-2024
\brief
This file contains the implementation for Sudoku
***************************************************************************/

#include "Sudoku.h"

/**
 * @brief Constructs a Sudoku solver instance.
 * 
 * @param _basesize The base size of the Sudoku puzzle (e.g., 3 for a 9x9 grid).
 * @param _stype The type of symbols used in the Sudoku puzzle (numbers or letters).
 * @param _callback A callback function that is called at certain events during solving.
 */
Sudoku::Sudoku(int _basesize, SymbolType _stype, SUDOKU_CALLBACK _callback)
    : m_SymbolType{_stype}, m_Callback{_callback}
{
  m_Length = _basesize * _basesize;
  m_Stats.basesize = _basesize;
  m_Board = new char[m_Length * m_Length];
}

/**
 * @brief Destructor that cleans up the dynamically allocated board.
 */
Sudoku::~Sudoku()
{
  delete[] m_Board;
}

/**
 * @brief Initializes the Sudoku board with given values.
 * 
 * @param _values Array containing the initial board values.
 * @param _size The size of the values array.
 */
void Sudoku::SetupBoard(const char *_values, size_t _size)
{
  for (size_t i = 0; i < _size; ++i)
    m_Board[i] = _values[i] == '.' ? EMPTY_CHAR : _values[i];
}

/**
 * @brief Attempts to place a value at the given row and column.
 * 
 * Recursively tries to solve the Sudoku puzzle by placing a value in an empty cell
 * and checking for the validity of this placement. If a valid placement cannot be
 * found, backtracks to previous placements.
 * 
 * @param _column The column index where the value is to be placed.
 * @param _row The row index where the value is to be placed.
 * @return true if the value can be placed; false otherwise.
 */
bool Sudoku::PlaceValue(unsigned _column, unsigned _row)
{
  // Calculate the linear index from column and row for one-dimensional board access
  unsigned linearIndex = static_cast<unsigned>(_column + m_Length * _row);

  // If we're at the end of a row, move to the next row, or end recursion if at the last row
  if (_row == m_Length) {
    return true;
  }

  // If the current cell is not empty, move to the next cell
  if (m_Board[linearIndex] != EMPTY_CHAR) {
    // Check if we're at the end of the row to wrap to the next row
    if (_column == m_Length - 1) {
      if (PlaceValue(0, _row + 1)) return true;
    } else {
      // Not at the end, simply move to the next column
      if (PlaceValue(_column + 1, _row)) return true;
    }
    // If neither condition is met, it means we cannot proceed and must backtrack
    return false;
  }

  // Start with the lowest possible value based on the symbol type
  char currentValue = m_SymbolType == SymbolType::SYM_NUMBER ? '1' : 'A';

  // Try placing all possible values in the current cell
  for (size_t i = 0; i < m_Length; ++i) {
    // Invoke the callback function to determine if we should abort the current attempt
    if (m_Callback(*this, m_Board, MessageType::MSG_ABORT_CHECK, m_Stats.moves, m_Stats.basesize, linearIndex, currentValue))
      return false;

    // Place the current value and update stats
    m_Board[linearIndex] = currentValue;
    ++m_Stats.moves;
    ++m_Stats.placed;
    // Inform the callback about the placement
    m_Callback(*this, m_Board, MessageType::MSG_PLACING, m_Stats.moves, m_Stats.basesize, linearIndex, currentValue);

    // If the current value is valid, proceed to place the next value
    if (CheckValidity(_column, _row, currentValue)) {
      // If we're at the end of the row, wrap to the next
      if (_column == m_Length - 1) {
        if (PlaceValue(0, _row + 1)) return true;
      } else {
        if (PlaceValue(_column + 1, _row)) return true;
      }
      // If the placement leads to a dead end, backtrack by resetting the cell
      m_Board[linearIndex] = EMPTY_CHAR;
      ++m_Stats.backtracks;
      // Inform the callback about the removal
      m_Callback(*this, m_Board, MessageType::MSG_REMOVING, m_Stats.moves, m_Stats.basesize, linearIndex, currentValue);
    }

    // If the value did not lead to a solution, reset the cell and decrement placed count
    m_Board[linearIndex] = EMPTY_CHAR;
    --m_Stats.placed;
    // Inform the callback about the removal
    m_Callback(*this, m_Board, MessageType::MSG_REMOVING, m_Stats.moves, m_Stats.basesize, linearIndex, currentValue);
    // Move to the next potential value
    ++currentValue;
  }

  // If no valid placement was found, return false to trigger backtracking
  return false;
}

/**
 * @brief Solves the Sudoku puzzle.
 * 
 * Initiates the recursive solving process by calling PlaceValue and
 * handles the callbacks for starting, successful completion, and failure.
 */
void Sudoku::Solve()
{
  unsigned x = 0;
  unsigned y = 0;

  m_Callback(*this, m_Board, MessageType::MSG_STARTING, m_Stats.moves, m_Stats.basesize, -1, 0);

  if (PlaceValue(x, y))
    m_Callback(*this, m_Board, MessageType::MSG_FINISHED_OK, m_Stats.moves, m_Stats.basesize, -1, 0);
  else
    m_Callback(*this, m_Board, MessageType::MSG_FINISHED_FAIL, m_Stats.moves, m_Stats.basesize, -1, 0);
}

/**
 * @brief Checks the validity of a value placed at a given row and column.
 * 
 * Verifies that the placement of the value does not conflict with the existing
 * values in the same row, column, or subgrid.
 * 
 * @param _column The column index of the cell to check.
 * @param _row The row index of the cell to check.
 * @param _value The value to check.
 * @return true if the value does not cause any conflicts; false otherwise.
 */
bool Sudoku::CheckValidity(unsigned _column, unsigned _row, char _value)
{
  // Calculate the linear index from column and row for one-dimensional board access
  // Add explicit casting to avoid warning for conversion from 'size_t' to 'unsigned int'
  unsigned linearIndex = static_cast<unsigned>(_column + static_cast<unsigned long>(m_Length) * _row);

  // Check for conflicts in the current row and column
  for (size_t i = 0; i < m_Length; ++i) {
    // Add explicit casting to avoid warning for conversion from 'size_t' to 'unsigned int'
    unsigned rowIndex = static_cast<unsigned>(i + static_cast<unsigned long>(m_Length) * _row); // Linear index for the row check
    unsigned colIndex = static_cast<unsigned>(_column + static_cast<unsigned long>(m_Length) * i); // Linear index for the column check

    // If the value exists elsewhere in the same row or column, return false
    if ((linearIndex != rowIndex) && (_value == m_Board[rowIndex])) return false;
    if ((linearIndex != colIndex) && (_value == m_Board[colIndex])) return false;
  }

  // Determine the maximum width and height of the subgrid containing the cell
  // The calculation is safe from overflow as _column and _row will not exceed the Sudoku grid size
  unsigned subgridMaxColumn = static_cast<unsigned>(_column / m_Stats.basesize) * m_Stats.basesize + m_Stats.basesize - 1;
  unsigned subgridMaxRow = static_cast<unsigned>(_row / m_Stats.basesize) * m_Stats.basesize + m_Stats.basesize - 1;

  // Determine the starting point for the subgrid
  unsigned subgridMinColumn = subgridMaxColumn - m_Stats.basesize + 1;
  unsigned subgridMinRow = subgridMaxRow - m_Stats.basesize + 1;

  // Check the subgrid for conflicts
  for (unsigned i = subgridMinRow; i <= subgridMaxRow; ++i) {
    for (unsigned j = subgridMinColumn; j <= subgridMaxColumn; ++j) {
      // Add explicit casting to avoid warning for conversion from 'size_t' to 'unsigned int'
      unsigned currentIndex = static_cast<unsigned>(j + static_cast<unsigned long>(m_Length) * i); // Linear index for the subgrid check
      
      // Skip the check for the cell itself and cells in the same row or column
      if (i == _row || j == _column) continue;

      // If the value exists elsewhere in the subgrid, return false
      if (m_Board[currentIndex] == _value) return false;
    }
  }
  
  // If no conflicts are found, the placement is valid
  return true;
}

/**
 * @brief Retrieves the current state of the Sudoku board.
 * 
 * @return A pointer to the character array representing the board.
 */
const char *Sudoku::GetBoard() const
{
  return m_Board;
}

/**
 * @brief Retrieves the statistics of the solving process.
 * 
 * @return A SudokuStats struct containing the statistics.
 */
Sudoku::SudokuStats Sudoku::GetStats() const
{
    return m_Stats;
}
