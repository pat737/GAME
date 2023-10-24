#include "Ball.h"
#include <vector>
#include <iostream>

Ball::Ball(sf::Vector2f position, float radius, float speed, sf::Color color)
{
	m_speed = speed;
	m_velocity.x = speed;
	m_velocity.y = speed;
	m_shape.setRadius(radius);
	m_shape.setPosition(position);
	m_shape.setFillColor(sf::Color::Red);
	m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
}

void Ball::draw(sf::RenderWindow& window)
{
	window.draw(m_shape);
}

void Ball::move(float dt, sf::RenderWindow& window)
{
	m_shape.move(m_velocity * dt);

	if (m_shape.getPosition().y - m_shape.getRadius() < 1 ||
		m_shape.getPosition().y + m_shape.getRadius() > window.getSize().y - 1)
		m_velocity.y = -m_velocity.y;
}

sf::Vector2f Ball::getPosition()
{
	return m_shape.getPosition();
}

void Ball::setPosition(float x, float y)
{
	m_shape.setPosition(x, y);
}

void Ball::reset(float x, float y, bool toTheLeft)
{
	setPosition(x, y);
	if (toTheLeft) m_velocity.x = -m_speed;
	else m_velocity.x = m_speed;
}

void Ball::collided()
{
	// change direction
	m_velocity.x = -m_velocity.x;
	updateVelocity(1.1f);
}

void Ball::updateVelocity(float val)
{
	m_velocity.x = m_velocity.x * val;
}

sf::CircleShape Ball::getShape()
{
	return m_shape;
}



