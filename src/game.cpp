#include "game.h"

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{

}

void Game::Init()
{
    LoadConfig();
}

void Game::Update(float dt)
{

}

void Game::ProcessInput(float dt)
{

}

void Game::Render()
{

}

void Game::LoadConfig()
{
    const std::string& filepath = "config.txt";
    std::ifstream stream(filepath);
    std::string line, word;

    while (getline(stream, line))
    {
        std::stringstream ss(line);
        ss >> word;

        if ( word == "Player")
        {
            ss >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.FR >>
                  m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >>
                  m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >>
                  m_playerConfig.V;
            ss >> m_playerConfig.S;
        }
        else if (word == "Enemy" )
        {
            ss >> m_enemyConfig.SR   >> m_enemyConfig.CR   >> m_enemyConfig.OR >>
                  m_enemyConfig.OG   >> m_enemyConfig.OB   >> m_enemyConfig.OT >>
                  m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >>
                  m_enemyConfig.SI;
            ss >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX;
        }
        else if (word == "Bullet")
        {
            ss >> m_bulletConfig.SR >> m_bulletConfig.CR  >> m_bulletConfig.FR >>
                  m_bulletConfig.FG >> m_bulletConfig.FB  >> m_bulletConfig.OR >>
                  m_bulletConfig.OG >> m_bulletConfig.OB  >> m_bulletConfig.OT >>
                  m_bulletConfig.V  >> m_bulletConfig.L;
            ss >> m_bulletConfig.S;
        }
    }
    std::cout << m_playerConfig.S << " " << m_enemyConfig.SMAX << " " << m_bulletConfig.S << std::endl;
}
