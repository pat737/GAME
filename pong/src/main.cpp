

// second time
#include <iostream>
#include <GameEngine.h>

int main()
{
	sf::RenderWindow window(sf::VideoMode(1200, 800), "MY PONG GAME", sf::Style::Default);
	window.setPosition(sf::Vector2i(100, 100));

	GameEngine gameEngine(window);
	gameEngine.run();
}