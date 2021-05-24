#ifndef _BUTTON_HPP_
#define _BUTTON_HPP_

#include <SFML/Graphics.hpp>

using namespace sf;



enum ButtonState { BTN_IDLE, BTN_HOVER, BTN_PRESSED };

class Button
{
public:
	Button (
			int x, 
			int y, 
			int width, 
			int height, 
			Font *font, 
			std::string text,
			Color idleColor,
			Color hoverColoer,
			Color activeColor 
	);

	
	bool isPressed();
	void render(RenderTarget *target);
	void update(const Vector2i& mousePosition);

private:
	RectangleShape m_shape;
	Font *m_font;
	Text m_text;

	ButtonState m_buttonState;
	Color m_idleColor;
	Color m_hoverColor;
	Color m_activeColor;

	int m_x;
	int m_y;


};
#endif