#include"game.h"
#include<fstream>
#include<iostream>

void applyGravity(std::shared_ptr<Entity> bh, std::shared_ptr<Entity> target)
{
	// 1. Calculate vector from target to black hole
	Vec2 direction = bh->cTransform->pos - target->cTransform->pos;
	float dist = direction.length();

	// 2. Only pull if they are within a certain range
	if (dist < 400.0f)
	{
		direction.normalize(); // Get the direction vector

		// You can make small enemies fly in faster because they are lighter!
		float strength = (target->tag() == "smallEnemy") ? 10.0f : 7.0f;

		target->cTransform->pos += direction * strength;
	}
}

Game::Game(const std::string& path)
	: m_text(m_font, ""),m_score(0) // Required: Initialize text with font
{
	init(path); // Load configs and setup window
}

void Game::init(const std::string& path)
{
	srand((unsigned int)time(NULL));
	std::ifstream fin(path);
	std::string name;

	while (fin >> name)
	{
		if (name == "Window")
		{
			fin >> m_w >> m_h >> framelimit >> IsFullscreen;


			if(IsFullscreen)
				m_window.create(sf::VideoMode({ (unsigned int)m_w, (unsigned int)m_h }), "Geometry Crash",sf::State::Fullscreen);
			else
				m_window.create(sf::VideoMode({ (unsigned int)m_w, (unsigned int)m_h }), "Geometry Crash", sf::Style::Default);


			m_window.setFramerateLimit(framelimit);

		}

		if (name == "Font")
		{
			std::string fontPath;
			int fontSize, r, b, g;

			fin >> fontPath >> fontSize >> r >> b >> g;

			if (!m_font.openFromFile(fontPath))
			{
				std::cerr << "Could not load font from: " << fontPath << std::endl;
				exit(-1);
			}

			//setting up text object
			m_text.setFont(m_font);
			m_text.setCharacterSize(fontSize);
			m_text.setFillColor(sf::Color(r, g, b));

			//set initial string 
			m_text.setString("Score: 0");

		}

		if (name == "Player")
		{
			fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR 
				>> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG 
				>> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;

		}

		if (name == "Enemy")
		{
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR
				>> m_enemyConfig.SMIN >> m_enemyConfig.SMAX
				>> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB
				>> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX
				>> m_enemyConfig.L >> m_enemyConfig.SI;

		}

		if (name == "Bullet")
		{
			fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S
				>> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB
				>> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB
				>> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;

		}

	}


	spawnPlayer();

}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

void Game::sMovement()
{
	if (!m_player) return;
	if (m_player && m_player->cInput)
	{
		m_player->cTransform->velocity = { 0, 0 };
		if (m_player->cInput->up) { m_player->cTransform->velocity.y -= m_playerConfig.S; }
		if (m_player->cInput->down) { m_player->cTransform->velocity.y += m_playerConfig.S; }
		if (m_player->cInput->left) { m_player->cTransform->velocity.x -= m_playerConfig.S; }
		if (m_player->cInput->right) { m_player->cTransform->velocity.x += m_playerConfig.S; }
	}

	for (auto e : m_entities.getEntities())
	{
		if (e->cTransform)
		{
			// Update position
			if (!m_paused)
				e->cTransform->pos += e->cTransform->velocity;

			// If it's a small enemy, maybe make it spin!
			if (e->tag() == "smallEnemy")
			{
				e->cTransform->angle += 6.0f; // Degrees per frame
			}
			else if (e->tag() != "Bullet")
			{
				e->cTransform->angle += 3.0f;
			}

			if (e->cTransform->angle >= 360.0f)
			{
				e->cTransform->angle -= 360.0f; // Keep it between 0-359
			}

			// Sync the shape
			if (e->cShape)
			{
				e->cShape->circle.setPosition(sf::Vector2f(e->cTransform->pos.x, e->cTransform->pos.y));
				e->cShape->circle.setRotation(sf::Angle(sf::degrees(e->cTransform->angle)));
			}
		}
	}

	for (auto bh : m_entities.getEntities("BlackHole"))
	{
		// Pull regular enemies
		for (auto e : m_entities.getEntities("Enemy"))
		{
			applyGravity(bh, e);
		}

		// Pull small enemies (fragments)
		for (auto se : m_entities.getEntities("smallEnemy"))
		{
			applyGravity(bh, se);
		}
	}
}

void Game::sUserInput()
{

	if (!m_player || !m_player->cInput) { return; }

	// pollEvent now returns an optional event
	while (const std::optional event = m_window.pollEvent())
	{
		// Use get_if<T>() to check the type of the event
		if (event->getIf<sf::Event::Closed>())
		{
			m_running = false;
		}

		// Handling Keyboard Pressed
		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		{
			if (keyPressed->code == sf::Keyboard::Key::W) { m_player->cInput->up = true; }
			if (keyPressed->code == sf::Keyboard::Key::S) { m_player->cInput->down = true; }
			if (keyPressed->code == sf::Keyboard::Key::A) { m_player->cInput->left = true; }
			if (keyPressed->code == sf::Keyboard::Key::D) { m_player->cInput->right = true; }
			if (keyPressed->code == sf::Keyboard::Key::P) { setPaused(!m_paused); }
			if (keyPressed->code == sf::Keyboard::Key::Escape) { m_running=false; }
			if (keyPressed->code == sf::Keyboard::Key::F) { toggleFullscreen(); }

			if (keyPressed->code == sf::Keyboard::Key::Space && m_gameState == "MENU")
			{
				m_gameState = "PLAYING";
				m_playerLives = 3;
				m_score = 0;

				if (!m_player)
				{
					spawnPlayer();
				}
			}

		}

		// Handling Keyboard Released
		if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
		{
			if (keyReleased->code == sf::Keyboard::Key::W) { m_player->cInput->up = false; }
			if (keyReleased->code == sf::Keyboard::Key::S) { m_player->cInput->down = false; }
			if (keyReleased->code == sf::Keyboard::Key::A) { m_player->cInput->left = false; }
			if (keyReleased->code == sf::Keyboard::Key::D) { m_player->cInput->right = false; }

		}

		// Mouse Button Events
		if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
		{
			if (mousePressed->button == sf::Mouse::Button::Left)
			{
				// In SFML 3, mouse position is included in the event data
				Vec2 mousePos((float)mousePressed->position.x, (float)mousePressed->position.y);
				spawnBullet(m_player, mousePos);
			}
			if (mousePressed->button == sf::Mouse::Button::Right)
			{
				if (m_currentFrame - m_lastSpecialAttackFrame >= m_specialAttackCooldown)
				{
					spawnBlackHole((float)mousePressed->position.x, (float)mousePressed->position.y); // Spawn at click position
					m_lastSpecialAttackFrame = m_currentFrame;
				}
				else
				{
					std::cout << "Special attack not ready";
				}
			}
		}
	}
}

void Game::sLifespan()
{
	for (auto e : m_entities.getEntities())
	{
		if (e->cLifespan)
		{
			// 1. Decrease remaining frames
			e->cLifespan->remaining -= 1;

			// 2. Calculate alpha (0 to 255)
			float ratio = (float)e->cLifespan->remaining / (float)e->cLifespan->total;
			unsigned char alpha = (unsigned char)(ratio * 255);

			// 3. Apply alpha to the shape's colors
			auto fill = e->cShape->circle.getFillColor();
			auto outline = e->cShape->circle.getOutlineColor();

			fill.a = alpha;
			outline.a = alpha;

			e->cShape->circle.setFillColor(fill);
			e->cShape->circle.setOutlineColor(outline);

			// 4. Destroy if expired
			if (e->cLifespan->remaining <= 0)
			{
				e->destroy();
			}
		}
	}
}

void Game::sRender()
{
	m_window.clear();

	if (m_gameState == "MENU")
	{
		m_text.setString("GEOMETRY CRASH");
		m_text.setCharacterSize(72);
		// Center the text
		m_window.draw(m_text);

		m_text.setString("PRESS 'SPACE' TO START");
		m_text.setCharacterSize(24);
		// Position below title
		m_window.draw(m_text);
	}

	else
	{
		// Draw Checkered Background
		int tileSize = 32; // Adjust size to match your image
		for (int x = 0; (unsigned int)x < m_window.getSize().x; x += tileSize)
		{
			for (int y = 0; (unsigned int)y < m_window.getSize().y; y += tileSize)
			{
				if ((x / tileSize + y / tileSize) % 2 == 0)
				{
					sf::RectangleShape tile(sf::Vector2f((float)tileSize, (float)tileSize));
					tile.setPosition(sf::Vector2f((float)x, (float)y));
					tile.setFillColor(sf::Color(10, 10, 10)); // Very dark grey
					m_window.draw(tile);
				}
			}
		}

		for (auto e : m_entities.getEntities()) 
		{
			if (e->tag() == "Player")
			{
				bool isInvincible = (m_currentFrame - m_lastDeathFrame < 120);

				// If invincible, only draw every other 5 frames (blinking effect)
				if (isInvincible && (m_currentFrame % 10 < 5))
				{
					// Skip drawing this frame
				}
				else
				{
					m_window.draw(m_player->cShape->circle);
				}

				continue;
			}
			if (e->cShape) // Only draw if it has a shape!
			{
				m_window.draw(e->cShape->circle);
			}
		}



		// Draw UI elements like the score last so they are on top
		std::string scoreStr = "Score: " + std::to_string(m_score);
		m_text.setString(scoreStr);
		m_text.setPosition(sf::Vector2f(10.f, 10.f));
		m_window.draw(m_text);

		m_text.setString("Lives: " + std::to_string(m_playerLives));
		m_text.setPosition(sf::Vector2f(20, 50));
		m_window.draw(m_text);
	}

	m_window.display();
}

void Game::sEnemySpawner()
{
	if (m_currentFrame - m_lastEnemySpawnTime >= m_enemyConfig.SI)
	{
		spawnEnemy();
		m_lastEnemySpawnTime = m_currentFrame;
	}
}

void Game::sCollision()
{
	//Collisions with Walls
	for (auto e : m_entities.getEntities())
	{
		if (e->cCollision && e->cTransform)
		{
			if (e->cTransform->pos.x < e->cCollision->radius ||
				e->cTransform->pos.x > m_window.getSize().x - e->cCollision->radius)
			{
				e->cTransform->velocity.x *= -1; // Reverse horizontal direction
				//e->cTransform->pos.x = e->cCollision->radius;
			}
			
			if (e->cTransform->pos.y<e->cCollision->radius ||
				e->cTransform->pos.y > m_window.getSize().y - e->cCollision->radius)
			{
				e->cTransform->velocity.y *= -1; //Reverse vertical direction
				//e->cTransform->pos.x = m_window.getSize().y - e->cCollision->radius;
			}
		}
	}

	//Vertical and Horizontal Clamping of player
	{
		//Horizontal Clamping
		if (m_player->cTransform->pos.x<m_player->cCollision->radius)
		{
			m_player->cTransform->pos.x = m_player->cCollision->radius;
		}
		else if (m_player->cTransform->pos.x > m_window.getSize().x - m_player->cCollision->radius)
		{
			m_player->cTransform->pos.x = (float)m_window.getSize().x - m_player->cCollision->radius;
		}
		//Vertical Clamping
		if (m_player->cTransform->pos.y<m_player->cCollision->radius)
		{
			m_player->cTransform->pos.y = m_player->cCollision->radius;
		}
		else if (m_player->cTransform->pos.y > m_window.getSize().y - m_player->cCollision->radius)
		{
			m_player->cTransform->pos.y = (float)m_window.getSize().y - m_player->cCollision->radius;
		}
	}

	//Collisions with bullet and enemy
	for (auto b : m_entities.getEntities("Bullet")) 
	{
		for (auto e : m_entities.getEntities("Enemy")) 
		{
			if (b->cTransform && e->cTransform && b->cCollision && e->cCollision)
			{
				float distSq = b->cTransform->pos.distSq(e->cTransform->pos);
				float radiiSum = b->cCollision->radius + e->cCollision->radius;

				// If squared distance is less than squared radii sum
				if (distSq < (radiiSum * radiiSum))
				{
					// Add score here!
					m_score += (int)e->cShape->circle.getPointCount()*100;
					b->destroy();
					e->destroy();
					spawnSmallEnemies(e);

				}
			}
		}
	}

	//collision with player and enemy
	for (auto e : m_entities.getEntities("Enemy"))
	{
		// Use the same distSq optimization we just discussed!
		float rSum = m_player->cCollision->radius + e->cCollision->radius;
		float dSq = m_player->cTransform->pos.distSq(e->cTransform->pos);

		if (dSq < (rSum * rSum))
		{
			//Destroy the enemy that hit the player
			e->destroy();
			//m_player->destroy();


			//Optional: Reset score or take a life
			 m_score -= 1000; 

			 int iframeDuration = 120; // 2 seconds at 60 FPS

			 if (m_currentFrame - m_lastDeathFrame > iframeDuration)
			 {
				 m_playerLives--;
				 m_lastDeathFrame = m_currentFrame; // Record the time of death

				 // Reset player position immediately
				 m_player->cTransform->pos = Vec2(m_w / 2.0f, m_h / 2.0f);

				 if (m_playerLives <= 0)
				 {
					 m_gameState = "MENU";
				 }
			 }

		}
	}

	//collision with smallEnemy and bullets
	for (auto b : m_entities.getEntities("Bullet"))
	{
		for (auto se : m_entities.getEntities("smallEnemy"))
		{
			float rSum = b->cCollision->radius + se->cCollision->radius;
			float dSq = b->cTransform->pos.distSq(se->cTransform->pos);

			if (dSq < (rSum * rSum))
			{
				m_score += (int)se->cShape->circle.getPointCount()*200; // Small enemies are often worth more points!
				b->destroy();
				se->destroy();
			}
		}
	}

	// collision with smallEnemy and player
	for (auto se : m_entities.getEntities("smallEnemy"))
	{
		float rSum = m_player->cCollision->radius + se->cCollision->radius;
		float dSq = m_player->cTransform->pos.distSq(se->cTransform->pos);

		if (dSq < (rSum * rSum))
		{
			se->destroy();
			m_player->destroy();
			spawnPlayer(); // Reset the player
			m_score -= 1000;
		}
	}

	//collision with black hole and enemy
	for (auto bh : m_entities.getEntities("BlackHole"))
	{
		for (auto e : m_entities.getEntities("Enemy"))
		{
			float distSq = bh->cTransform->pos.distSq(e->cTransform->pos);
			float pullRadius = 30.0f; // Center of the black hole

			if (distSq < (pullRadius * pullRadius))
			{
				e->destroy(); // Sucked in!
				spawnSmallEnemies(e);
				m_score += 500; // Bonus points
			}
		}
	}

	//collision with black hole and smallenemy
	for (auto bh : m_entities.getEntities("BlackHole"))
	{
		for (auto se : m_entities.getEntities("smallEnemy"))
		{
			float distSq = bh->cTransform->pos.distSq(se->cTransform->pos);
			if (distSq < (30.0f * 30.0f)) // Collision radius
			{
				se->destroy();
				m_score += 200; // Reward for cleaning up fragments
			}
		}
	}
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	// 1. Get big enemy data
	int vertices = (int)e->cShape->circle.getPointCount();
	float angleStep = 360.0f / vertices;

	for (int i = 0; i < vertices; i++)
	{
		auto small = m_entities.addEntity("smallEnemy");

		// 2. Calculate velocity vector
		float angle = i * angleStep * (3.14159f / 180.0f); // Convert to radians
		Vec2 velocity(std::cos(angle), std::sin(angle));
		velocity *= e->cTransform->velocity.length(); // Scale speed as desired

		// 3. Attach components
		small->cTransform = std::make_shared<CTransform>(e->cTransform->pos, velocity, 0.0f);
		small->cShape = std::make_shared<CShape>(e->cCollision->radius / 2.0f, vertices,
			e->cShape->circle.getFillColor(), e->cShape->circle.getOutlineColor(),
			e->cShape->circle.getOutlineThickness());

		// 4. Important: Add the Lifespan!
		small->cLifespan = std::make_shared<CLifespan>(120); 

		small->cCollision = std::make_shared<CCollision>(e->cCollision->radius / 2.0f);
	}
}

void Game::spawnPlayer()
{
	m_player= m_entities.addEntity("Player");

	float px = (float)m_window.getSize().x * 0.5f;
	float py = (float)m_window.getSize().y * 0.5f;

	m_player->cTransform = std::make_shared<CTransform>(Vec2(px, py), Vec2(0.0f, 0.0f), 0.0f);

	m_player->cShape = std::make_shared<CShape>(
		m_playerConfig.SR, m_playerConfig.V,
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), // Usually enemies have no fill or a random one
		sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
		m_playerConfig.OT
	);
	
	m_player->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
	m_player->cInput = std::make_shared<CInput>();
}

void Game::spawnEnemy()
{
	// 1. Get a random speed
	float speed = m_enemyConfig.SMIN + (float(rand()) / RAND_MAX) * (m_enemyConfig.SMAX - m_enemyConfig.SMIN);

	// 2. Pick a random angle in radians (0 to 2*PI)
	float angle = (float(rand()) / RAND_MAX) * 2.0f * 3.14159f;

	// 3. Convert Polar coordinates (speed/angle) to Cartesian (X/Y)
	float vx = cos(angle) * speed;
	float vy = sin(angle) * speed;

	Vec2 velocity(vx, vy);

	auto enemy = m_entities.addEntity("Enemy");

	int r = rand() % 255;
	int g = rand() % 255;
	int b = rand() % 255;
	sf::Color randomFill(r, g, b);

	// Position should also be random within window bounds
	float ex = (float)(m_enemyConfig.SR + (rand() % ((int)m_window.getSize().x - (2 * m_enemyConfig.SR))));
	float ey = (float)(m_enemyConfig.SR + (rand() % ((int)m_window.getSize().y - (2 * m_enemyConfig.SR))));

	enemy->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), velocity, 0.0f);

	int vertices = m_enemyConfig.VMIN + (rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1));

	enemy->cShape = std::make_shared<CShape>(
		m_enemyConfig.SR, vertices,
		randomFill, // Usually enemies have no fill or a random one
		sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
		m_enemyConfig.OT
	);

	// 4. Setup Collision
	enemy->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos)
{
	auto bullet = m_entities.addEntity("Bullet");

	// 1. Calculate the direction vector from player to mouse
	Vec2 direction = mousePos - m_player->cTransform->pos;

	// 2. Normalize the direction and multiply by the config speed
	// Note: Ensure your Vec2 class has a normalize() or length() function
	direction.normalize();
	Vec2 velocity = direction * m_bulletConfig.S;

	// 3. Attach components
	bullet->cTransform = std::make_shared<CTransform>(m_player->cTransform->pos, velocity, 0.0f);

	bullet->cShape = std::make_shared<CShape>(
		m_bulletConfig.SR, m_bulletConfig.V,
		sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
		sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
		m_bulletConfig.OT
	);

	// 4. Setup Collision and Lifespan
	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
	bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::run()
{
	while (m_running)
	{
		// 1. The Manager adds/removes entities first
		m_entities.update();

		// 2. Logic that should stop when paused
		if (m_gameState == "PLAYING" && !m_paused)
		{
			sEnemySpawner(); // Decides if it's time for a new enemy
			sCollision();    // Checks for hits and wall bounces
			sLifespan();     // Fades out bullets and fragments
		}

		sMovement();     // Moves Player and Enemies
		// 3. Systems that must always run
		sRender();           // Draws the current state to the window
		sUserInput();        // Listens for 'P' to unpause even if paused!

		// 4. Increment the frame counter
		m_currentFrame++;
	}
}

void Game::toggleFullscreen()
{
	// 1. Determine the new state
	bool currentlyFull = IsFullscreen; // Simplified check

	// 2. Recreate the window
	if (currentlyFull)
	{
		m_window.create(sf::VideoMode({ (unsigned int)m_w, (unsigned int)m_h }), "Geometry Crash", sf::Style::Default);
	}
	else
	{
		m_window.create(sf::VideoMode({ (unsigned int)m_w, (unsigned int)m_h }), "Geometry Crash", sf::State::Fullscreen);
	}

	// 3. IMPORTANT: Reset the framerate limit after recreating
	m_window.setFramerateLimit(framelimit);
}

void Game::spawnBlackHole(float x, float y)
{
	auto bh = m_entities.addEntity("BlackHole");
	bh->cTransform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0);
	bh->cShape = std::make_shared<CShape>(30.0f, 32, sf::Color::Black, sf::Color::Magenta, 4);
	bh->cCollision = std::make_shared<CCollision>(100.0f); // Large pull radius
	bh->cLifespan = std::make_shared<CLifespan>(200); // Lasts for 200 frames
}