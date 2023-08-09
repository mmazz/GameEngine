#include "game.h"


// Game-related State data
SpriteRenderer  *Renderer;
GameObject      *Player;
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_POSITION(50.0f, 50.0f);
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 45.5f;
BallObject     *Ball;
TextRenderer  *Text;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Ball;
}

void Game::Init()
{
    LoadConfig();
    // load shaders
    ResourceManager::LoadTexture("textures/background2.jpg", false, "background");
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // load textures
    ResourceManager::LoadTexture("textures/ryu.png", true, "player");
    ResourceManager::LoadTexture("textures/ball.png", true, "ball");
    ResourceManager::LoadTexture("textures/power.png", true, "power");
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height/ 2.0f - PLAYER_SIZE.y / 2.0f);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("player"));
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
                                              -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("ball"));
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("fonts/ocraext.TTF", 24);
}

void Game::ResetScore()
{
    this->Score= 0;
}

void Game::ResetPlayer()
{
    Player->Position.x = this->Width / 2.0f - PLAYER_SIZE.x / 2.0f;
    Player->Position.y = this->Height/ 2.0f - PLAYER_SIZE.y / 2.0f;
}
void Game::ResetBalls()
{
    Ball->Reset(INITIAL_BALL_POSITION,  INITIAL_BALL_VELOCITY);
}
void Game::Update(float dt)
{
    Ball->Move(dt, this->Width, this->Height);
         // check for collisions
    this->DoCollisions();
    if (Ball->Position.y >= this->Height) // did ball reach bottom edge?
    {
        --this->Score;
        // did the player lose all his lives? : Game over
        if (this->Score== 0)
        {
            this->ResetScore();
            this->State = GAME_MENU;
        }
    }
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
                Player->Position.x -= velocity;
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
                Player->Position.x += velocity;
        }
        if (this->Keys[GLFW_KEY_W])
        {
            if (Player->Position.y >= 0.0f )
                Player->Position.y -= velocity;
        }
        if (this->Keys[GLFW_KEY_S])
        {
            if (Player->Position.y <= this->Height - Player->Size.y)
                Player->Position.y += velocity;
        }
        if (this->Keys[GLFW_MOUSE_BUTTON_LEFT])
        {
        }
    }
}

void Game::Render()
{
    if(this->State == GAME_ACTIVE)
    {
        // draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"),
            glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        std::stringstream ss; ss << this->Score;
        Text->RenderText("Score:" + ss.str(), 5.0f, 5.0f, 1.0f);
    }
    Player->Draw(*Renderer);
    Ball->Draw(*Renderer);
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


// collision detection
bool CheckCollision(GameObject &one, GameObject &two);
bool CheckCollision(BallObject &one, GameObject &two);

void Game::DoCollisions()
{

    // check collisions for player pad (unless stuck)
    bool result = CheckCollision(*Ball, *Player);
    if (result)
    {

        this->ResetScore();
        this->ResetPlayer();
        this->ResetBalls();
    }
}

bool CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

bool CheckCollision(BallObject &one, GameObject &two)
{
    // get center point circle first
    glm::vec2 center(one.Position + one.Radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // now that we know the clamped values, add this to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // now retrieve vector between center circle and closest point AABB and check if length < radius
    difference = closest - center;

    if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
        return true;
    else
        return false;

}

