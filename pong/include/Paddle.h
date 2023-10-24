#include <SFML/Graphics.hpp>
#include <iostream>

class Paddle
{
private:
	float m_speed = 200.f;
	sf::Vector2f m_size;
	sf::RectangleShape m_shape;

public:
	enum Direction { none, up, down };

	Paddle(sf::Vector2f position, float width, float height, sf::Color color);
	void draw(sf::RenderWindow& window);
	void move(float dt, float windowYVal);
	void move(float dt, Direction d);

	sf::FloatRect getBounds() const;
	sf::RectangleShape getShape();

	void setSpeed(float speed);
};

