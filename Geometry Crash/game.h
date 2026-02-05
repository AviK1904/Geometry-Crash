#pragma once

#include"EntityManager.h"
#include<SFML/Graphics.hpp>


struct PlayerConfig
{
	int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; 
	float S;
};

struct EnemyConfig 
{ 
	int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; 
	float SMIN, SMAX; 
};

struct BulletConfig 
{ 
	int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; 
	float S; 
};

class Game
{
    sf::RenderWindow    m_window;       // The main game window
    EntityManager       m_entities;     // Our manager for all game objects
    sf::Font            m_font;         // Font for score and UI
    sf::Text            m_text;         // Text object for drawing score
    PlayerConfig        m_playerConfig;
    EnemyConfig         m_enemyConfig;
    BulletConfig        m_bulletConfig;
    int                 m_score = 0;
    int                 m_currentFrame = 0;
    int                 m_lastEnemySpawnTime = 0;
    bool                m_paused = false;
    bool                m_running = true;
    int                 m_playerLives = 3;
    std::string         m_gameState = "MENU";
    
    // Window Variables
    int                 m_w, m_h, framelimit, IsFullscreen;

    //Special Attack 
    int                 m_lastSpecialAttackFrame = 0, m_specialAttackCooldown = 300;
    int                 m_lastDeathFrame = 0;

    //Direct pointer to Player
    std::shared_ptr<Entity> m_player;

    void init(const std::string& path); // Parse config and create window
    void setPaused(bool paused);          // Pause logic

    // Systems
    void sMovement();                     // Update entity positions
    void sUserInput();                    // Handle keyboard/mouse
    void sLifespan();                     // Handle entity "death" timers
    void sRender();                       // Draw everything to screen
    void sEnemySpawner();                 // Spawn enemies at intervals
    void sCollision();                    // Handle all collisions

    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
    void toggleFullscreen();
    void spawnBlackHole(float x, float y);

public:
    Game(const std::string& config);
    void run();
};

