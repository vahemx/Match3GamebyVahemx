#include "Matcher.hpp"
#include "MatchPatterns.hpp"
#include "GameController.hpp"


Matcher::Matcher(Grid &grid)
	: m_grid(grid)
{

}

void Matcher::initMatcher(GameController *pController)
{
	m_pGameController = pController;
	generateMatchPatterns();
	
	generatePreMatchPatterns();
}

bool Matcher::match()
{
	bool isMatch3 = match3();
	bool isMatchPattern = matchCustomPatterns();

	return ( isMatch3 || isMatchPattern);
}

bool Matcher::preMatch3()
{
	bool ret = false;
	for (int i = 0; i < m_pGameController->m_rows; ++i)
	{
		for (int j = 0; j < m_pGameController->m_columns; ++j)
		{
			if (m_grid[i][j].getType() == BOMB_MATCH_TYPE)
			{
				return true;
			}
		}
	}

	ret = matchCustomPatterns(true);



	return ret;
}

bool Matcher::match3()
{
	bool isMatch = false;
	for (int i = 0; i < m_pGameController->m_rows; ++i)
	{
		for (int j = 0; j < m_pGameController->m_columns; ++j)
		{
			if (i < m_pGameController->m_rows - 2)
			{
				if (m_grid[i][j].getType() == m_grid[i + 1][j].getType() &&
					m_grid[i][j].getType() == m_grid[i + 2][j].getType())
				{
					isMatch = true;
					bool isBombMatch = true;
					for (int n = 0; n < 3; ++n)
					{
						m_grid[i + n][j].setMatch(true);
						if (m_grid[i + n][j].getBombType() <= NB)
						{
							isBombMatch = false;
						}
					}
					if (isBombMatch)
					{
						for (int n = 0; n < 3; ++n)
						{
							m_grid[i + n][j].setExplode(true);
							m_pGameController->explodeBomb(i + n, j);
						}
					}
				}
			}

			if (j < m_pGameController->m_columns - 2)
			{
				if (m_grid[i][j].getType() == m_grid[i][j + 1].getType() &&
					m_grid[i][j].getType() == m_grid[i][j + 2].getType())
				{
					isMatch = true;
					bool isBombMatch = true;
					for (int n = 0; n < 3; ++n)
					{
						m_grid[i][j + n].setMatch(true);
						if (m_grid[i][j + n].getBombType() <= NB)
						{
							isBombMatch = false;
						}
					}

					if (isBombMatch)
					{
						for (int n = 0; n < 3; ++n)
						{
							m_grid[i][j + n].setExplode(true);
							m_pGameController->explodeBomb(i, j + n);
						}
					}
				}
			}

		}
	}
	return isMatch;
}

bool Matcher::matchCustomPatterns(bool isPreMatchPattern)
{
	bool ret = false;
	const MatchPatterns &patterns = isPreMatchPattern ? m_preMatchPatterns : m_matchPatterns;
	for (const auto &item : patterns)
	{

		for (int i = 0; i < m_pGameController->m_rows; ++i)
		{
			for (int j = 0; j < m_pGameController->m_columns; ++j)
			{
				if (  i + item.size() <= static_cast<unsigned int>(m_pGameController->m_rows) && j + item[0].size() <= static_cast<unsigned int> (m_pGameController->m_columns) )
				{

					
					int type = -1;
					bool isMatch = true;
					bool isPatternElementCounted = false;
					for (size_t m = 0; m < item.size(); ++m)
					{
						for (size_t n = 0; n < item[m].size(); ++n)
						{
							if (item[m][n] == 0)
								continue;
							else
							{
								if (type == -1)
								{
									type = m_grid[i + m][j + n].getType();
								}
								
								if ( m_grid[i + m][j + n].getCounted() )
								{
									isPatternElementCounted = true;
								}
								if (m_grid[i + m][j + n].getType() != type)
								{
									isMatch = false;
									break;
								}
							}
							
						}

						if (!isMatch)
						{
							break;
						}
					}

					if (isMatch)
					{
						if (isPreMatchPattern == true)
						{
							return true;
						}
						else
						{
							BombType bombType = getBombTypeFromMatchPattern(item);
							ret = true;
							bool isBombSet = false;
							bool isBombMatch = false;
							for (size_t m = 0; m < item.size(); ++m)
							{
								for (size_t n = 0; n < item[m].size(); ++n)
								{
									if (item[m][n] != 0)
									{
										Vector2i matchedColRow = { j + static_cast<int> (n), i + static_cast<int> (m) };
										isBombMatch = (m_grid[matchedColRow.y][matchedColRow.x].getType() == BOMB_MATCH_TYPE);
										m_grid[i + m][j + n].setMatch(true);

										if (isBombMatch)
										{
											m_grid[i + m][j + n].setExplode(true);
											m_pGameController->explodeBomb(i + m, j + n);
										}
										else
										{
											if (m_pGameController->m_isSwapped && !isBombSet)
											{
												if (matchedColRow == m_pGameController->m_sel1 ||
													matchedColRow == m_pGameController->m_sel2)
												{
													if (!m_grid[i + m][j + n].getCounted())
													{
														m_grid[i + m][j + n].setBombType(bombType);
														isBombSet = true;
													}
												}
											}
											else
											{
												if (!isBombSet && !isPatternElementCounted)
												{
													if (!m_grid[i + m][j + n].getCounted())
													{
														m_grid[i + m][j + n].setBombType(bombType);
														isBombSet = true;
													}
												}
											}
										}

										m_grid[i + m][j + n].setCounted(true);
									}
								}
							}
						}

					}
					
				}
			}
		}
	}
	
	return ret;
}

void Matcher::generateMatchPatterns()
{
	for (auto &item : matchPatterns)
	{
		Pattern2D pattern = item;
		m_matchPatterns.push_back(pattern);
		int n = getNumRotations(pattern);
		for (int i = 0; i < n; ++i)
		{
			rotateMatrix(pattern);
			m_matchPatterns.push_back(pattern);
		}
	}
}


///TODO:
///generate pre match patterns to determine if a player has legal move to make a match. If not, the grid should be shuffled 
void Matcher::generatePreMatchPatterns()
{
	Pattern2D preMatch3 = 
	{
		{ NB, NB, NB},
		{ 0,  0,  0},
	};

	for (unsigned int i = 0; i < preMatch3[0].size(); ++i)
	{
		if (preMatch3[0][i] > 0 && preMatch3[1][i] == 0)
		{
			Pattern2D matrix = preMatch3;
			std::swap(matrix[0][i], matrix[1][i]);
			m_preMatchPatterns.push_back(matrix);
			for (int j = 0; j < 3; ++j)
			{
				rotateMatrix(matrix);
				m_preMatchPatterns.push_back(matrix);
			}
		}
	}
}

void Matcher::rotateMatrix(Pattern2D& matrix)
{
	const int M = matrix[0].size(); 
	const int N = matrix.size();

	Pattern2D rotated_matrix;
	rotated_matrix.resize(M);
	for (int x = 0; x < M; ++x) {
		rotated_matrix[x].resize(N);
	}
	rotateMatrixImpl(matrix, rotated_matrix, M, N);
	matrix = rotated_matrix;
}

void Matcher::rotateMatrixImpl(const Pattern2D &originalMatrix, Pattern2D& rotatedMatrix, const int M, const int N)
{
	for (int x = 0; x < N; ++x) {
		for (int y = 0; y < M; ++y) {
			rotatedMatrix[y][-x - 1 + N] = originalMatrix[x][y];
		}
	}
}


int Matcher::getNumRotations(const Pattern2D& pattern)
{
	int numRotations = 0;
	if (pattern.size() == 1)
	{
		numRotations = 1;
	}
	else if (!patternIsSquare(pattern))
	{
		numRotations = 3;
	}

	return numRotations;

}

bool Matcher::patternIsSquare(const Pattern2D & pattern)
{
	int compare;
	int arraySize = pattern.size();
	if (arraySize > 0 && pattern[0].size() > 0)
	{
		compare = pattern[0][0];
	}
	else
	{
		return false;
	}

	for (size_t i = 0; i < pattern.size(); ++i)
	{
		int subArraySize = pattern[i].size();
		if (subArraySize != arraySize)
		{
			return false;
		}


		for (size_t j = 0; j < pattern[i].size(); ++j)
		{
			if (compare != pattern[i][j])
			{
				return false;
			}
		}
	}

	return true;
}

BombType Matcher::getBombTypeFromMatchPattern(const Pattern2D& pattern)
{
	int rows = pattern.size();
	int cols = pattern[0].size();
	if (rows == 1 || cols == 1)
	{
		bool isHorizontal = (pattern.size() == 1);
		if (pattern[0][0] == BombType::LB)
		{
			if (isHorizontal)
			{
				return BombType::VB;
			}
			else
			{
				return BombType::HB;
			}
		}
		else
		{
			return static_cast<BombType> (pattern[0][0]);
		}
	}

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			if (pattern[i][j] == 0)
			{
				continue;
			}
			else
			{
				return static_cast<BombType> (pattern[i][j]);
			}
		}
	}


	return BombType::NB;
}