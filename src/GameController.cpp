#include <SFML/Graphics.hpp>
#include "GameController.hpp"
#include "GemElement.hpp"


using namespace sf;

GameController::GameController()
: m_matcher(m_grid)
{
	m_moveCount = m_config.getMoveCount();
	m_rows = m_config.getRowsCount();
	m_columns = m_config.getColumnsCount();
	m_gemsSize = m_config.getGemColorsCount();
}

void GameController::updateGameStatus(const GameStatus &status) {
    _gameStatus = status;
}

void GameController::startGame() {
    _app = new RenderWindow(VideoMode(744, 1080), "Game", Style::Close);
    _app->setFramerateLimit(60);


    run();
}

void GameController::run() {
	init();
	updateGameStatus(GameStatus::Started);
    while (_app->isOpen()) {
        _app->clear(Color(55, 70, 70, 255));
		if (m_state == State::WAITING)
		{
			if (isObjectiveCompleted())
			{
				updateGameStatus(GameStatus::Won);
			}
			else if (m_moveCount == 0)
			{
				updateGameStatus(GameStatus::Failed);
			}
		}

        sf::Event event;
        while (_app->pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                _app->close();
			if (event.type == sf::Event::MouseButtonPressed && _gameStatus == GameStatus::Started)
			{
				if (event.key.code == Mouse::Left)
				{
					if (m_state != State::MOVING && !m_isSwapped)
					{
						Vector2i position = Mouse::getPosition(*_app);
						position -= m_offset;
						checkMouseHit(position);
						if (m_state == State::SWAPPING)
						{
							m_explode = false;
							m_started = true;
							swapGems(m_sel1, m_sel2);
							m_isSwapped = true;
							if (m_grid[m_sel1.y][m_sel1.x].getBombType() > NB)
							{
								m_explode = true;
								m_grid[m_sel1.y][m_sel1.x].setExplode(true);
								explodeBomb(m_sel1.y, m_sel1.x);
							}

							if (m_grid[m_sel2.y][m_sel2.x].getBombType() > NB)
							{
								m_explode = true;
								m_grid[m_sel2.y][m_sel2.x].setExplode(true);
								explodeBomb(m_sel2.y, m_sel2.x);
							}
						}
						else if (m_state == State::DOUBLE_CLICK)
						{
							m_started = true;
							m_explode = false;
							int row = m_sel1.y;
							int col = m_sel1.x;

							BombType bombType = m_grid[row][col].getBombType();
							if (bombType > NB)
							{
								--m_moveCount;
								m_explode = true;
								m_grid[row][col].setExplode(true);
								explodeBomb(row, col);
							}
						}
					}
					
				}
			}
        }

		update();
		drawTopBar();
		drawMoveCountAndObjectives();
		drawGridTiles();
		drawGemSprites();

		if (_gameStatus != GameStatus::Started)
		{
			updateFinalSceneAndReset();
			updateMousePosition();
			m_restartButton->update(m_mousePositionView);
			if (m_restartButton->isPressed())
			{
				resetGame();
			}
		}
		

        _app->display();
    }
}

void GameController::updateFinalSceneAndReset()
{
	if ((_gameStatus == GameStatus::Won ||
		_gameStatus == GameStatus::Failed) )
	{
		Text endingText;
		endingText.setFont(m_font);
		endingText.setString(_gameStatus == GameStatus::Won ? "You Win!" : "You Lose!");
		Vector2f appSize = _app->getDefaultView().getSize();
		int size = static_cast<int> (appSize.x / 10);
		endingText.setCharacterSize(size);
		FloatRect textSize = endingText.getLocalBounds();
		int x = static_cast<int> (_app->getDefaultView().getSize().x / 2 - textSize.width / 2 - textSize.left);
		int y = static_cast<int> (m_topBarSize.y + (m_offset.y - m_topBarSize.y) / 2 - textSize.height / 2 - textSize.top);
		endingText.setPosition(static_cast<float> (x), static_cast<float> (y));
		endingText.setOutlineColor(_gameStatus == GameStatus::Won ? Color::Green : Color::Red);
		endingText.setOutlineThickness(5);

		
		m_restartButton->render(_app);

		_app->draw(endingText);

	}
}

void GameController::updateMousePosition()
{
	m_mousePositionView = Mouse::getPosition(*_app);
}

void GameController::resetGame()
{
	m_grid.clear();
	initGrid();
	m_started = false;
	m_alpha = 255;
	resetObjectives();
	m_moveCount = m_config.getMoveCount();;
	m_state = State::WAITING;
	_gameStatus = GameStatus::Started;
	m_isSwapped = false;
	m_explode = false;

}


void GameController::resetObjectives()
{
	std::map<std::string, int> objectivesMap = m_config.getObjectivesMap();
	int i = 0;
	for (auto & item : objectivesMap)
	{
		m_gemObjectiveScoreVec[i] = item.second;
		++i;
	}
}

void GameController::init()
{
	srand((unsigned int)time(0));
	initTextures();
	initOffset();
	initGrid();
	m_matcher.initMatcher(this);
	initButton();
}

void GameController::initButton()
{
	int buttonWidth = 200;
	int buttonHeight = 50;

	int buttonX = static_cast<int> (_app->getDefaultView().getSize().x / 2 - buttonWidth / 2);
	int buttonY = static_cast<int> (m_rows * m_gridTextures[0].getSize().y / 2 - buttonHeight / 2 + m_offset.y);
	m_restartButton = std::unique_ptr<Button> (new Button(buttonX, buttonY, buttonWidth, buttonHeight, &m_font, "Play Again", Color(130, 130, 130), Color(110, 110, 110), Color(90, 90, 90)));
}

void GameController::initTextures()
{
	Texture gridTile_1;
	gridTile_1.loadFromFile("../../Resources/tile_1.png");

	Texture gridTile_2;
	gridTile_2.loadFromFile("../../Resources/tile_2.png");

	m_gridTextures.push_back(gridTile_1);
	m_gridTextures.push_back(gridTile_2);

	m_gemTexturesPathMap["red"].loadFromFile("../../Resources/red.png");
	m_gemTexturesPathMap["blue"].loadFromFile("../../Resources/blue.png");
	m_gemTexturesPathMap["orange"].loadFromFile("../../Resources/orange.png");
	m_gemTexturesPathMap["green"].loadFromFile("../../Resources/green.png");
	m_gemTexturesPathMap["violet"].loadFromFile("../../Resources/violet.png");
	

	std::map<std::string, int> objectivesMap = m_config.getObjectivesMap();
	m_gemScoreHist.resize(m_gemsSize, 0);

	for (auto & item : objectivesMap)
	{
		m_gemTexturesVec.push_back(m_gemTexturesPathMap[item.first]);
		m_gemObjectiveScoreVec.push_back(item.second);
	}

	int objMapSize = objectivesMap.size();
	int i = objMapSize;
	for (auto &item : m_gemTexturesPathMap)
	{
		if (objectivesMap.find(item.first) == objectivesMap.end() && i < m_gemsSize)
		{
			m_gemTexturesVec.push_back(item.second);
			i++;
		}
	}

	m_font.loadFromFile("../../Resources/MilkyNice.ttf");

	initBombTextures();
	
}

void GameController::initBombTextures()
{
	m_bombTextures.resize(3);
	m_bombTextures[0].loadFromFile("../../Resources/h_bomb.png");
	m_bombTextures[1].loadFromFile("../../Resources/v_bomb.png");
	m_bombTextures[2].loadFromFile("../../Resources/bomb.png");
}

void GameController::initOffset()
{
	const int gridTileWidth = m_gridTextures[0].getSize().x;
	const int gridwidth = gridTileWidth * m_columns;
	Vector2f appSize = _app->getDefaultView().getSize();
	int topBarHeight = static_cast<int>(appSize.y) * TOP_BAR_HEIGHT_SIZE_RATIO / 100;

	m_offset = { static_cast<int> ((appSize.x - gridwidth) / 2), static_cast<int> ((topBarHeight + TOP_BAR_HEIGHT_OFFSET)) };
	m_topBarSize = { static_cast<int> (appSize.x), static_cast<int> (topBarHeight) };
	int scoreRectWidth = 200;
	int scoreRectHeight = static_cast<int> (topBarHeight);
	m_scoreRectSize = { scoreRectWidth , scoreRectHeight };
	m_scoreRectOffset = { (m_topBarSize.x / 2) - (m_scoreRectSize.x / 2), 0 };
} 

void GameController::initGrid()
{
	for (int i = 0; i < m_rows; ++i)
	{
		std::vector <GemElement> ColumnVec;
		for (int j = 0; j < m_columns; ++j)
		{
			int gridIndex = (i & 1) ? (j & 1) : !(j & 1);
			int spriteType = rand() % m_gemsSize;
			Sprite sprite(m_gemTexturesVec[spriteType]);
			int tileWidth = m_gridTextures[gridIndex].getSize().x;
			int tileHeight = m_gridTextures[gridIndex].getSize().y;
			Vector2i gridTileCoords = { j * tileWidth, i * tileHeight };
			Vector2i spriteOffset = getSpriteOffset(sprite, tileWidth, tileHeight);
			Vector2i spriteCoords = gridTileCoords;
			GemElement elem(sprite, i, j, gridTileCoords.x, gridTileCoords.y, spriteType, spriteOffset);
			ColumnVec.push_back(elem);
		}
		m_grid.push_back(ColumnVec);
	}
}

Vector2i GameController::getSpriteOffset(const Sprite& sprite, const int &tileWidth, const int &tileHeight)
{
	Vector2i spriteOffset;
	spriteOffset.x = (tileWidth - sprite.getTexture()->getSize().x) / 2;
	spriteOffset.y = (tileHeight - sprite.getTexture()->getSize().y) / 2;

	return spriteOffset;

}

void GameController::drawGridTiles()
{
	for (int i = 0; i < m_rows; ++i)
	{
		for (int j = 0; j < m_columns; ++j)
		{
			int gridIndex = (i & 1) ? (j & 1) : !(j & 1);
			Sprite sprite(m_gridTextures[gridIndex]);
			int tileWidth = m_gridTextures[gridIndex].getSize().x;
			int tileHeight = m_gridTextures[gridIndex].getSize().y;
			Vector2i gridTileCoords = { j * tileWidth, i * tileHeight };

			sprite.setPosition(static_cast<float> (gridTileCoords.x), static_cast<float> (gridTileCoords.y));
			sprite.move(static_cast<float> (m_offset.x), static_cast<float> (m_offset.y));
			_app->draw(sprite);
		}
	}
}

void GameController::drawGemSprites()
{
	for (auto &items : m_grid)
	{
		for (auto & gem : items)
		{
			auto &sprite = gem.getSprite();
			float x = static_cast<float> (gem.getCoords().x);
			float y = static_cast<float> (gem.getCoords().y);
			int alpha = gem.alpha;
			int visibilityOffset = m_gridTextures[0].getSize().y / 3;
			if (y < -visibilityOffset)
			{
				alpha = 0;
			}

			sprite.setColor(Color(255, 255, 255, alpha));
			if (_gameStatus != GameStatus::Started)
			{

				if (m_alpha > 90)
				{
					--m_alpha;
				}
				sprite.setColor(Color(255, 255, 255, (y < -visibilityOffset) ? alpha : m_alpha));
			}

				
			sprite.setPosition(x, y);
			sprite.move(static_cast<float> (m_offset.x + gem.getOffset().x), static_cast<float> (m_offset.y + gem.getOffset().y));
			_app->draw(sprite);
		}
	}
}

void GameController::drawTopBar()
{
	sf::RectangleShape topBar({ static_cast<float> (m_topBarSize.x), static_cast<float> (m_topBarSize.y) });
	topBar.setFillColor(sf::Color(150, 150, 150, 255));
	_app->draw(topBar);
}

void GameController::drawMoveCountAndObjectives()
{
	sf::RectangleShape scoreRect({ static_cast<float> (m_scoreRectSize.x), static_cast<float> (m_scoreRectSize.y) });
	scoreRect.setPosition(static_cast<float> (m_scoreRectOffset.x), static_cast<float> (m_scoreRectOffset.y));
	scoreRect.setFillColor(sf::Color(100, 100, 100, 255));
	_app->draw(scoreRect);

	drawMoveCountText();
	drawObjectives();
}

void GameController::drawObjectives()
{
	sf::Text text;
	text.setFont(m_font);
	int objectiveElementsTotalWidth = 0;
	int interElementXoffset = 15;
	Vector2f position(0.f, 0.f);

	int textCharacterSize = static_cast<int> ((m_scoreRectSize.y / 2) / 1.5f);
	text.setCharacterSize(textCharacterSize);
	int objectiveCount = m_config.getObjectivesMap().size();

	for (int i = 0; i < objectiveCount; ++i)
	{
		objectiveElementsTotalWidth += (m_gemTexturesVec[i].getSize().x + interElementXoffset);
	}

	int startXOffset = (m_scoreRectOffset.x - objectiveElementsTotalWidth) / 2;
	Vector2f spriteOffset = { static_cast<float> (startXOffset), 0 };
	for (int i = 0; i < objectiveCount; ++i)
	{
		Sprite sprite;
		sprite.setTexture(m_gemTexturesVec[i]);
		std::string num = std::to_string(m_gemObjectiveScoreVec[i]);
		text.setString(num);
		int startYoffset = (m_scoreRectSize.y / 2 - m_gemTexturesVec[i].getSize().y) / 2;
		spriteOffset.y = static_cast<float> (startYoffset);
		sprite.setPosition(position + spriteOffset);

		sf::FloatRect textRect = text.getLocalBounds();
		int textXOffset = static_cast<int> ((m_gemTexturesVec[i].getSize().x - textRect.width) / 2 - textRect.left);
		int textYOffset = static_cast<int> ( m_scoreRectSize.y / 2 + (m_scoreRectSize.y / 2 - textRect.height) / 2 - textRect.top);
		text.setPosition( position.x + spriteOffset.x + textXOffset, static_cast<float> (textYOffset));

		position.x += (m_gemTexturesVec[i].getSize().x + interElementXoffset);
		_app->draw(sprite);
		_app->draw(text);
	}
}

void GameController::drawMoveCountText()
{
	sf::Text text;
	text.setFont(m_font); 
	std::string moveCount = std::to_string(m_moveCount);
	text.setString(moveCount);
	int textSize = static_cast<int> (m_scoreRectSize.y / 1.5f);
	text.setCharacterSize(textSize);

	sf::FloatRect textRect = text.getLocalBounds();
	Vector2f offset = { (m_scoreRectSize.x - textRect.width) / 2 - textRect.left, (m_scoreRectSize.y - textRect.height) / 2 - textRect.top };

	text.setPosition(static_cast<float> (m_scoreRectOffset.x) + offset.x, static_cast<float> (m_scoreRectOffset.y) + offset.y);
	_app->draw(text);
}


void GameController::checkMouseHit(Vector2i position)
{
	if (position.x > 0 && position.y > 0)
	{
		int gridRow = position.y / m_gridTextures[0].getSize().y;
		int gridCol = position.x / m_gridTextures[0].getSize().x;
		if (gridRow > m_rows - 1 || gridCol > m_columns - 1)
		{
			m_state = State::WAITING;
		}
		else
		{
			if (m_state == State::WAITING)
			{
				m_sel1.x = position.x / m_gridTextures[0].getSize().x;
				m_sel1.y = position.y / m_gridTextures[0].getSize().y;
				m_state = State::SELECTED;
			}
			else if (m_state == State::SELECTED)
			{
				m_sel2.x = position.x / m_gridTextures[0].getSize().x;
				m_sel2.y = position.y / m_gridTextures[0].getSize().y;
				if (abs(m_sel2.x - m_sel1.x) + abs(m_sel2.y - m_sel1.y) == 1)
				{
					m_state = State::SWAPPING;
				}
				else if (abs(m_sel2.x - m_sel1.x) + abs(m_sel2.y - m_sel1.y) == 0)
				{
					m_state = State::DOUBLE_CLICK;
				}
				else
				{
					m_state = State::WAITING;
				}
			}
		}
	}
	else
	{
		m_state = State::WAITING;
	}
}

void GameController::swapGems(Vector2i a, Vector2i b)
{
	GemElement elem_1 = m_grid[a.y][a.x];
	GemElement elem_2 = m_grid[b.y][b.x];
	GemElement temp = elem_1;

	elem_1.setRow(elem_2.getRow());
	elem_1.setColumn(elem_2.getColumn());

	elem_2.setRow(temp.getRow());
	elem_2.setColumn(temp.getColumn());

	m_grid[b.y][b.x] = elem_1;
	m_grid[a.y][a.x] = elem_2;
}

void GameController::update()
{
	animateMovement();
	
	bool isMatch = doMatch();
	if (!isMatch)
	{
		bool isPreMatch = doPreMatch();
		if (!isPreMatch)
		{
			m_shuffle = true;
		}
	}
	

	if (m_state != State::MOVING && m_isSwapped)
	{
		if (!isMatch && !m_explode)
		{
			swapGems(m_sel1, m_sel2);
		}
		else
		{
			--m_moveCount;
		}
		m_isSwapped = false;
		m_explode = false;
	}
	

	animateDelete();

	if (computeScore())
	{
		updateObjectiveCounts();
	}

	reArrange();
}

void GameController::updateObjectiveCounts()
{
	int objectiveCount = m_config.getObjectivesMap().size();

	for (int i = 0; i < objectiveCount; ++i)
	{
		m_gemObjectiveScoreVec[i] -= m_gemScoreHist[i];
		if (m_gemObjectiveScoreVec[i] < 0)
		{
			m_gemObjectiveScoreVec[i] = 0;
		}
	}
}

bool GameController::isObjectiveCompleted()
{
	if (std::find_if(m_gemObjectiveScoreVec.begin(), m_gemObjectiveScoreVec.end(), [] (const int &value) { return value != 0; }) != m_gemObjectiveScoreVec.end())
	{
		return false;
	}

	return true;
}

bool GameController::doMatch()
{
	return m_matcher.match();
}

bool GameController::doPreMatch()
{
	return m_matcher.preMatch3();
}

bool GameController::computeScore()
{
	bool ret = false;

	if (m_state != State::MOVING && m_started)
	{
		std::fill(m_gemScoreHist.begin(), m_gemScoreHist.end(), 0);
		int score = 0;
		for (int i = 0; i < m_rows; ++i)
		{
			for (int j = 0; j < m_columns; ++j)
			{
				if (m_grid[i][j].getMatch())
				{
					if (m_grid[i][j].getType() != BOMB_MATCH_TYPE)
					{
						m_gemScoreHist[m_grid[i][j].getType()]++;
						ret = true;
					}
				}
			}
		}
	}

	return ret;
}

void GameController::animateMovement()
{
	bool foundMove = false;
	for (int i = 0; i < m_rows; ++i)
	{
		for (int j = 0; j < m_columns; ++j)
		{
			int gridIndex = (i & 1) ? (j & 1) : !(j & 1);
			GemElement &elem = m_grid[i][j];
			Vector2i newCoords = elem.getCoords();
			int deltaX;
			int deltaY;
			for (int n = 0; n < GAME_MOVEMENT_SPEED; ++n)
			{
				deltaX = elem.getCoords().x - elem.getColumn() * m_gridTextures[gridIndex].getSize().x;
				deltaY = elem.getCoords().y - elem.getRow() * m_gridTextures[gridIndex].getSize().y;

				if (deltaX != 0)
				{
					newCoords.x -= deltaX > 0 ? 1 : -1;
				}
				if (deltaY != 0)
				{
					newCoords.y -= deltaY > 0 ? 1 : -1;
				}
				elem.setCoords(newCoords);
			}
			if (deltaX != 0 || deltaY != 0)
			{
				foundMove = true;
				m_state = State::MOVING;
			}
		}
	}

	if (!foundMove && m_state == State::MOVING)
	{
		m_state = State::WAITING;
	}
}

void GameController::animateDelete()
{
	if (m_state != State::MOVING)
	{
		for (int i = 0; i < m_rows; ++i)
		{
			for (int j = 0; j < m_columns; ++j)
			{
				if (m_grid[i][j].getMatch())
				{
					for (int n = 0; n < GAME_MOVEMENT_SPEED; ++n)
					{
						if (m_grid[i][j].alpha > 5)
						{
							m_grid[i][j].alpha -= 5;
							m_state = State::MOVING;;
						}
					}
				
				}
			}
		}
	}
}

void GameController::reArrange()
{
	if (m_state != State::MOVING)
	{
		if (m_shuffle == true)
		{
			m_shuffle = false;
			shuffle();
		}
		else
		{
			for (int i = m_rows - 1; i >= 0; --i)
			{
				for (int j = 0; j < m_columns; ++j)
				{
					if (m_grid[i][j].getMatch() && (m_grid[i][j].getBombType() <= NB || m_grid[i][j].getExplode()))
					{
						for (int n = i; n >= 0; n--)
						{
							if (!m_grid[n][j].getMatch() || (m_grid[n][j].getBombType() > NB && !m_grid[n][j].getExplode()))
							{
								swapGems({ j, n }, { j, i });
								break;
							}
						}
					}
				}
			}


			for (int j = 0; j < m_columns; ++j)
			{
				for (int i = m_rows - 1, n = 1; i >= 0; i--)
				{
					if (m_grid[i][j].getMatch())
					{
						int gridIndex = (i & 1) ? (j & 1) : !(j & 1);
						int tileWidth = m_gridTextures[gridIndex].getSize().x;
						int tileHeight = m_gridTextures[gridIndex].getSize().y;
						int spriteType = rand() % m_gemsSize;
						Sprite sprite;

						BombType bombType = m_grid[i][j].getBombType();
						bool explode = m_grid[i][j].getExplode();
						if (bombType > NB && !explode)
						{
							m_grid[i][j].setMatch(false);
							m_grid[i][j].setType(BOMB_MATCH_TYPE);
							int textureIndex = getBombTextureIndex(bombType);
							sprite.setTexture(m_bombTextures[textureIndex]);
						}
						else
						{
							m_grid[i][j].setType(spriteType);
							sprite.setTexture(m_gemTexturesVec[spriteType]);
							m_grid[i][j].setBombType(NB);
						}


						m_grid[i][j].setSprite(sprite);

						Vector2i spriteOffset = getSpriteOffset(m_grid[i][j].getSprite(), tileWidth, tileHeight);
						m_grid[i][j].setOffset(spriteOffset);
						m_grid[i][j].alpha = 255;
						if (m_grid[i][j].getBombType() <= NB)
						{
							Vector2i coords = m_grid[i][j].getCoords();
							coords.y = -static_cast<int> (m_gridTextures[gridIndex].getSize().y) * n++;
							m_grid[i][j].setCoords({ coords.x, coords.y });
						}

						m_grid[i][j].setCounted(false);
						m_grid[i][j].setMatch(false);
						m_grid[i][j].setCounted(false);
						m_grid[i][j].setExplode(false);

					}
				}
			}
		}		
		if (m_state == State::DOUBLE_CLICK)
		{
			m_state = State::WAITING;
		}
	}
}

void GameController::shuffle()
{
	for (int i = 0; i < m_rows; ++i)
	{
		for (int j = 0; j < m_columns; ++j)
		{
			int row = rand() % m_rows;
			int col = rand() % m_columns;
			swapGems({ j, i }, { col, row });
		}
	}
}

int GameController::getBombTextureIndex(BombType type)
{
	switch (type)
	{
	case VB:
		return VERTICAL;
	case HB:
		return HORIZONTAL;
	case RB:
		return RADIAL;
	default:
		return -1;
	}
}

void GameController::explodeBomb(int row, int col)
{
	bool explodeRow = false;
	bool explodeCol = false;
	m_grid[row][col].setExplode(true);
	BombType btype = m_grid[row][col].getBombType();
	if (btype == RB)
	{
		explodeRow = true;
		explodeCol = true;
	}
	else if (btype == VB)
	{
		explodeCol = true;
	}
	else if (btype == HB)
	{
		explodeRow = true;
	}

	if (explodeRow)
	{
		for (int j = 0; j < m_columns; ++j)
		{
			m_grid[row][j].setMatch(true);
			if (m_grid[row][j].getBombType() > NB && j != col)
			{
				if (!m_grid[row][j].getExplode())
					explodeBomb(row, j);
			}
		}
	}

	if (explodeCol)
	{
		for (int i = 0; i < m_rows; ++i)
		{
			m_grid[i][col].setMatch(true);
			if (m_grid[i][col].getBombType() > NB && i != row)
			{
				if (!m_grid[i][col].getExplode())
					explodeBomb(i, col);
			}
		}
	}
}


