
#ifndef GameController_hpp
#define GameController_hpp

#include <SFML/Graphics.hpp>
#include "Match3GameDefs.hpp"
#include "Matcher.hpp"
#include "Button.hpp"
#include "Config.hpp"
#include <stdio.h>

using namespace sf;

class GameController {

public :
	enum class State : int
	{
		WAITING, SELECTED, SWAPPING, MOVING, DOUBLE_CLICK
	};

    GameController();

	friend class Matcher;

    void startGame();

    void updateGameStatus(const GameStatus &status);

private:

    GameStatus _gameStatus = NotStarted;
    RenderWindow *_app;
	Grid m_grid;
	void init();
	void initButton();
	void initTextures();
	void initBombTextures();
	void initOffset();
	void initGrid();
	void resetGame();
	void resetObjectives();
	Vector2i getSpriteOffset(const Sprite & sprite, const int& tileWidth, const int& tileHeight);
	void drawGridTiles();
	void drawGemSprites();
	void drawTopBar();
	void checkMouseHit(Vector2i position);
	void swapGems(Vector2i a, Vector2i b);
	void update();
	void updateFinalSceneAndReset();
	bool doMatch();
	bool doPreMatch();
	void animateMovement();
	void animateDelete();
	void reArrange();
	void shuffle();
	bool computeScore();
	void updateObjectiveCounts();
	void updateMousePosition();
	bool isObjectiveCompleted();
	int getBombTextureIndex(BombType type);
	void explodeBomb(int row, int col);
	void drawMoveCountAndObjectives();
	void drawObjectives();
	void drawMoveCountText();
	
	std::map<std::string, Texture> m_gemTexturesPathMap;
	std::map<int, std::string> m_gemTypeIndexMap;
	std::vector<int> m_gemObjectiveScoreVec;
	std::vector<int> m_gemScoreHist;

	std::vector<Texture> m_gridTextures;
	std::vector<Texture> m_gemTexturesVec;
	std::vector<Texture> m_bombTextures;
	sf::Font m_font;
	std::unique_ptr<Button> m_restartButton;
	Config m_config;
	Vector2i m_topBarSize;
	Vector2i m_scoreRectSize;
	Vector2i m_scoreRectOffset;
	Vector2i m_offset;
	Vector2i m_sel1;
	Vector2i m_sel2;
	Vector2i m_mousePositionView;
	int m_alpha = 255;
	Matcher m_matcher;
	bool m_started = false;
	bool m_isSwapped = false;
	bool m_explode = false;
	bool m_shuffle = false;
	int m_moveCount;
	int m_rows;
	int m_columns;
	int m_gemsSize;
	State m_state = State::WAITING;


    void run();


};

#endif