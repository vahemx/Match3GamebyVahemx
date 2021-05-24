#include "Button.hpp"



Button::Button(
	int x,
	int y,
	int width,
	int height,
	Font *font,
	std::string text,
	Color idleColor,
	Color hoverColor,
	Color activeColor
)
{
	m_buttonState = BTN_IDLE;
	m_shape.setSize(Vector2f(static_cast<float> (width),static_cast<float> (height) ));
	m_shape.setPosition(Vector2f(static_cast<float> (x), static_cast<float> (y) ));
	m_font = font;
	m_text.setFont(*m_font);
	m_text.setString(text);
	m_text.setFillColor(Color::Green);
	m_idleColor = idleColor;
	m_hoverColor = hoverColor;
	m_activeColor = activeColor;
	m_shape.setFillColor(m_idleColor);
	m_shape.setOutlineColor(Color::White);
	m_shape.setOutlineThickness(2);
	sf::FloatRect textSize = m_text.getLocalBounds();
	int textX = static_cast<int> (x + width / 2 - textSize.width / 2 - textSize.left);
	int textY = static_cast<int> (y + height / 2 - textSize.height / 2 - textSize.top);
	m_text.setPosition(static_cast<float> (textX), static_cast<float> (textY));
	m_x = x;
	m_y = y;

}

void Button::render(RenderTarget *target)
{
	target->draw(m_shape);
	target->draw(m_text);
}

void Button::update(const Vector2i &mousePosition)
{
	m_buttonState = BTN_IDLE;
	Vector2i position = { mousePosition.x - m_x, mousePosition.y - m_y };

	if (position.x > 0 && position.y > 0)
	{
		Vector2f shapeSize = m_shape.getSize();
		int x = static_cast<int> (position.x / shapeSize.x);
		int y = static_cast<int> (position.y / shapeSize.y);

		if (x == 0 && y == 0)
		{

			m_buttonState = BTN_HOVER;
			if (Mouse::isButtonPressed(Mouse::Left))
			{
				m_buttonState = BTN_PRESSED;
			}
		}
	}


	if (m_buttonState == BTN_IDLE)
	{
		m_shape.setFillColor(m_idleColor);
	}
	else if (m_buttonState == BTN_HOVER)
	{
		m_shape.setFillColor(m_hoverColor);
	}
	else if (m_buttonState == BTN_PRESSED)
	{
		m_shape.setFillColor(m_activeColor);
	}
}

bool Button::isPressed()
{
	return (m_buttonState == BTN_PRESSED);
}