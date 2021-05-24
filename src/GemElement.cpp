#include "GemElement.hpp"


GemElement::GemElement(Sprite &sprite, int& row, int& col, int &x, int &y, int&  type, Vector2i offset)
	: m_sprite(sprite),
	m_row(row),
	m_column(col),
	m_x(x),
	m_y(y),
	m_type(type),
	m_spriteOffset(offset),
	m_bomb(BombType::NB)
{}

int GemElement::getType()
{
	return m_type;
}

Vector2i GemElement::getCoords()
{
	return { m_x, m_y };
}

Sprite GemElement::getSprite()
{
	return m_sprite;
}

int GemElement::getRow()
{
	return m_row;
}

int GemElement::getColumn()
{
	return m_column;
}

Vector2i GemElement::getOffset()
{
	return m_spriteOffset;
}

void GemElement::setRow(const int& row)
{
	m_row = row;
}

void GemElement::setColumn(const int& col)
{
	m_column = col;
}

void GemElement::setCoords(Vector2i coords)
{
	m_x = coords.x;
	m_y = coords.y;
}


void GemElement::setMatch(bool match)
{
	m_match = match;
}

bool GemElement::getMatch()
{
	return m_match;
}

void GemElement::setType(int type)
{
	m_type = type;
}

void GemElement::setOffset(Vector2i offset)
{
	m_spriteOffset = offset;
}

void GemElement::setSprite(const Sprite &sprite)
{
	m_sprite = sprite;
}

BombType GemElement::getBombType()
{
	return m_bomb;
}

void GemElement::setBombType(BombType type)
{
	m_bomb = type;
}

void GemElement::setCounted(bool counted)
{
	m_counted = counted;
}

bool GemElement::getCounted()
{
	return m_counted;
}

void GemElement::setExplode(bool explode)
{
	m_explode = explode;
}

bool GemElement::getExplode()
{
	return m_explode;
}