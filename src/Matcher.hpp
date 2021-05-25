#ifndef _MATCHER_HPP
#define _MATCHER_HPP

#include "Match3GameDefs.hpp"

using Pattern2D = std::vector<std::vector<int> >;

class GameController;

class Matcher
{
public:
	Matcher(Grid &grid);
	void initMatcher(GameController *pController);
	bool match();
	bool match3();
	bool preMatch3();
	bool matchCustomPatterns(bool isPreMatchPattern = false);
	BombType getBombTypeFromMatchPattern(const Pattern2D& pattern);
	void generateMatchPatterns();
	void generatePreMatchPatterns();
	int getNumRotations(const Pattern2D& pattern);
	void Matcher::rotateMatrix(Pattern2D& matrix);

private:
	void rotateMatrixImpl(const Pattern2D &originalMatrix, Pattern2D& rotatedMatrix, const int M, const int N);
	bool patternIsSquare(const Pattern2D & pattern);
	Grid &m_grid;
	MatchPatterns m_matchPatterns;
	MatchPatterns m_preMatchPatterns;
	GameController *m_pGameController;
};






#endif