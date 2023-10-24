#include "GameEngine.h"

GameEngine::GameEngine(sf::RenderWindow& window)
	: m_window(window),
	m_paddle1(sf::Vector2f(20, window.getSize().y / 2.f), 10, 100, sf::Color::White),
	m_paddle2(sf::Vector2f(window.getSize().x - 20.f, window.getSize().y - 100.f), 10, 100, sf::Color::White),
	m_ball(sf::Vector2f(window.getSize().x / 2.f, window.getSize().y / 2.f), 25, 200.f, sf::Color::White)
{
	m_p1Score = 0;
	m_p2Score = 0;
	m_gStates = GameStates::intro;
	m_font.loadFromFile(".\\assets\\fonts\\digital-7.ttf");
	m_hud.setFont(m_font);
	m_hud.setCharacterSize(50);
	m_hud.setFillColor(sf::Color::White);
	m_hud.setPosition((m_window.getSize().x / 2.f) - 45.f, 10);

	menuText.setFont(m_font);
	menuText.setCharacterSize(50);
	menuText.setFillColor(sf::Color::White);
	menuText.setPosition((m_window.getSize().x / 2.f) - 45.f, 10);

	m_paddle1.setSpeed(1000.f);

	m_music.openFromFile(".\\assets\\audio\\pong.wav");
	m_music.setRelativeToListener(false);
	m_music.setLoop(true);

	m_ballBuffer.loadFromFile(".\\assets\\audio\\beep.flac");
	m_ballSound.setBuffer(m_ballBuffer);
	m_ballSound.setAttenuation(1.f);
}

void GameEngine::draw()
{
	m_window.clear();
	m_paddle1.draw(m_window);
	m_paddle2.draw(m_window);
	m_ball.draw(m_window);
	m_window.draw(m_hud);
	m_window.display();
}

void GameEngine::update()
{
	// update hud
	std::stringstream ss;
	switch (m_gStates)
	{
	case GameEngine::intro:
		ss << "Press the Space\nkey to start";
		break;
	case GameEngine::playing:
		ss << m_p1Score << " - " << m_p2Score;
		break;
	case GameEngine::gameOver:
		if (m_p1Score > m_p2Score) {
			ss << "Player 1 wins";
		}
		else {
			ss << "Player 2 wins";
		}
		break;
	default:
		break;
	}

	m_hud.setString(ss.str());
}

void GameEngine::run()
{
	float dt;

	Paddle::Direction plyDir;
	Paddle::Direction npcDir;
	plyDir = Paddle::Direction::none;
	npcDir = Paddle::Direction::none;

	bool music = true;
	m_music.play();

	while (m_window.isOpen())
	{
		dt = m_clock.restart().asSeconds();

		// INTRO
		if (m_gStates == GameStates::intro) {
			sf::Event event;
			while (m_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed) m_window.close();
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
					m_window.close();
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
					m_gStates = GameStates::playing;
					m_music.stop();
				}
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M) {
					music = !music;
					if (!music) m_music.stop();
					else if (m_music.getStatus() != sf::Music::Playing) m_music.play();
				}
			}

			// update hud
			update();
			std::string m;
			if (music) m = "\n\n\n Music ON (M)";
			else m = "\n\n\n Music OFF (M)";

			menuText.setString(m_hud.getString() + m);
			menuText.setPosition(m_window.getSize().x / 2.f - 200.f, m_window.getSize().y / 2.f - 100.f);

			m_window.clear();
			m_window.draw(menuText);
			m_window.display();
		}

		// PLAYING
		if (m_gStates == GameStates::playing) {
			sf::Vector2f ballPos = m_ball.getPosition();
			// PADDLE 1 (PLAYER)
			sf::Event event;
			while (m_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed) m_window.close();
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
					m_window.close();

				// move up
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W)
					plyDir = Paddle::Direction::up;
				// move down
				else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)
					plyDir = Paddle::Direction::down;
			}

			// stop movement if none of the keys are pressed
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S)) plyDir = Paddle::Direction::none;

			// stop movement at top and bottom
			float paddle1Center = m_paddle1.getBounds().top + m_paddle1.getBounds().height / 2.f;
			if (plyDir == Paddle::Direction::up && paddle1Center < 1.f)
				plyDir = Paddle::Direction::none;
			if (plyDir == Paddle::Direction::down && paddle1Center > m_window.getSize().y - 1.f)
				plyDir = Paddle::Direction::none;

			// move user paddle
			m_paddle1.move(dt, plyDir);

			// PADDLE 2 (AI)
			float paddle2Center = m_paddle2.getBounds().top + m_paddle2.getBounds().height / 2.f;
			sf::Vector2f viewCenter = m_window.getView().getCenter();
			if (ballPos.x < viewCenter.x) { // if ball on the other side -> move to the center
				if (paddle2Center < viewCenter.y) npcDir = Paddle::Direction::down;
				else npcDir = Paddle::Direction::up;
			}
			else { // ball on paddle2 side
				if (m_ball.getPosition().y + 10.f < paddle2Center) // up
					npcDir = Paddle::Direction::up;
				else if (m_ball.getPosition().y - 10.f > paddle2Center) // down
					npcDir = Paddle::Direction::down;
				else npcDir = Paddle::Direction::none;
			}

			// stop movement at top and bottom
			if (npcDir == Paddle::Direction::up && paddle2Center < 1.f)
				npcDir = Paddle::Direction::none;
			if (npcDir == Paddle::Direction::down && paddle2Center > m_window.getSize().y - 1.f)
				npcDir = Paddle::Direction::none;

			// move NPC paddle
			m_paddle2.move(dt, npcDir);

			// BALL
			m_ball.move(dt, m_window);

			// SCORES
			if (ballPos.x < 1.f) {
				m_p2Score++;
				m_ball.reset(m_window.getSize().x / 2.f, m_window.getSize().y / 2.f, false);
			}
			else if (ballPos.x > m_window.getView().getCenter().x + m_window.getSize().x / 2.f) {
				m_p1Score++;
				m_ball.reset(m_window.getSize().x / 2.f, m_window.getSize().y / 2.f, true);
			}

			// COLLISIONS
			sf::Vector2f leftPart_ball(ballPos.x - m_ball.getShape().getRadius(), ballPos.y);
			sf::Vector2f rightPart_ball(ballPos.x + m_ball.getShape().getRadius(), ballPos.y);
			if (m_paddle1.getBounds().intersects(m_ball.getShape().getGlobalBounds()) || m_paddle2.getBounds().intersects(m_ball.getShape().getGlobalBounds())) {
				m_ballSound.play();
				m_ball.collided();
			}

			if (m_p1Score == 3 || m_p2Score == 3) {
				m_gStates = gameOver;
				m_music.play();
			}

			// update hud
			update();
			// draw shapes to screen
			draw();
		}

		// GAME OVER
		if (m_gStates == GameStates::gameOver) {
			sf::Event event;
			while (m_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed) m_window.close();
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
					m_window.close();
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
					m_gStates = GameStates::playing;
					m_music.stop();
					m_p1Score = 0;
					m_p2Score = 0;
				}
			}

			// update hud
			update();
			std::string m = "\nPress Space to play again\nor Escape to Exit.";

			menuText.setString(m_hud.getString() + m);
			menuText.setPosition(m_window.getSize().x / 2.f - 200.f, m_window.getSize().y / 2.f - 100.f);

			m_window.clear();
			m_window.draw(menuText);
			m_window.display();
		}

	}
}