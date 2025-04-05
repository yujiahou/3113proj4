#include "LevelC.hpp"
#include "Utility.hpp"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH[] = "assets/Pixel_Sprite.jpeg",//https://openclipart.org/detail/215080/retro-character-sprite-sheet
           ENEMY_FILEPATH[]       = "assets/ghost.png";//https://www.freeiconspng.com/img/12480


unsigned int LEVELC_DATA[] =
{
    100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    120, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0,
    120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 44,
    120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 18, 18, 19,
    120, 0, 0, 0, 0, 0, 0, 0, 17, 18, 38, 38, 38, 39,
    120, 0, 0, 0, 0, 0, 0, 0, 37, 38, 38, 38, 38, 39
};



LevelC::~LevelC()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    
}

void LevelC::initialise()
{
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/tilemap_packed.png");//Created/distributed by Kenney (www.kenney.nl)
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 20, 9);
    
    // Code from main.cpp's initialise()

    // Existing
    int player_walking_animation[4][4] =
    {
        { 8, 9, 10, 11 },
        { 4, 5, 6, 7 },
        { 12, 13, 14, 15 },
        { 0, 1, 2, 3 }
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        1.0f,                      // width
        1.0f,                       // height
        PLAYER
    );
        
    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(3.0f);
    
    /**
    Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] =  Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, BOUNCE, IDLE);
        m_game_state.enemies[i].set_jumping_power(3.0f);
    }

    m_game_state.enemies[0].set_position(glm::vec3(8.0f, -4.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/tap.wav");
    m_game_state.attack_sfx = Mix_LoadWAV("assets/power_up.wav");
    m_game_state.win_sfx = Mix_LoadWAV("assets/coin.wav");
    //MUSIC by Brackeys, Sofia Thirslund
    
}

void LevelC::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    
    if (m_game_state.player->get_position().y < -10.0f) m_game_state.next_scene_id = 1;
    for (int i = 0; i < ENEMY_COUNT; i++) m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
    
}

void LevelC::render(ShaderProgram *program)
{
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++)    m_game_state.enemies[i].render(program);
    
}
