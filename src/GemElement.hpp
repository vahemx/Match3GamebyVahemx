#ifndef _GEM_ELEMENT_HPP
#define _GEM_ELEMENT_HPP

#include <SFML/Graphics.hpp>
//#include "Match3GameDefs.hpp"

using namespace sf;

/// Bomb Types:
/// NB - bomb type not available;
/// LB - Linear Bomb type
///	HB - Horizontal Bomb type
/// VB - Vertical Bomb type
/// RB - Radial Bomb type

enum BombType :int { NB = 1, LB, HB, VB, RB };

class GemElement
{
public:
	GemElement(Sprite& sprite, int& row, int& col, int &x, int &y, int& type, Vector2i offset);
	Vector2i getCoords();
	void setCoords(Vector2i coords);
	Vector2i getOffset();
	Sprite getSprite();
	int getRow();
	int getColumn();
	int getType();
	void setType(int type);
	void setRow(const int& row);
	void setColumn(const int& column);
	void setMatch(bool match);
	void setCounted(bool counted);
	void setExplode(bool explode);
	bool getExplode();
	bool getCounted();
	bool getMatch();
	BombType getBombType();
	void setBombType(BombType type);
	void setOffset(Vector2i offset);
	void setSprite(const Sprite &sprite);
	int alpha = 255;
private:
	Sprite m_sprite;
	int m_row;
	int m_column;
	int m_x;
	int m_y;
	bool m_match = false;
	bool m_counted = false;
	bool m_explode = false;
	Vector2i m_spriteOffset;
	BombType m_bomb;
	int m_type;

};

#endif

