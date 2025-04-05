/**
* Author: Yujia Hou
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LIVES 3
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.hpp"
#include "Utility.hpp"
#include "Scene.hpp"
#include "LevelA.hpp"
#include "Menu.hpp"
#include "LevelB.hpp"
#include "LevelC.hpp"
#include "Lose.hpp"
#include "Win.hpp"
#include "Effects.hpp"

// ––––– CONSTANTS ––––– //
constexpr int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

constexpr int CD_QUAL_FREQ    = 44100,
          AUDIO_CHAN_AMT  = 2,
          AUDIO_BUFF_SIZE = 4096;

constexpr char BGM_FILEPATH[] = "assets/Morning.mp3";//"Morning" Kevin MacLeod (incompetech.com)
constexpr int    LOOP_FOREVER = -1;

Mix_Music *g_music;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
constexpr char FONT_FILEPATH[] = "assets/font1.png";

enum AppStatus { RUNNING, TERMINATED };

// ––––– GLOBAL VARIABLES ––––– //
Scene  *g_current_scene;
Menu   *g_menu;
LevelA *g_levelA;
LevelB *g_levelB;
LevelC *g_levelC;
Lose   *g_lose;
Win    *g_win;

Effects *g_effects;
Scene   *g_levels[6];

SDL_Window* g_display_window;


ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;
glm::vec3 win_pos = glm::vec3(13.0f, -4.0f, 0.0f);

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool g_is_colliding_bottom = false;
bool lose = false;
bool win = false;
bool pause = false;
float lives = LIVES;
GLuint font_texture_id;

AppStatus g_app_status = RUNNING;

void swtich_to_scene(Scene *scene);
void initialise();
void process_input();
void update();
void render();
void shutdown();

// ––––– GENERAL FUNCTIONS ––––– //
void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise(); // DON'T FORGET THIS STEP!
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Special Effects!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    

        Mix_OpenAudio(
            CD_QUAL_FREQ,
            MIX_DEFAULT_FORMAT,
            AUDIO_CHAN_AMT,
            AUDIO_BUFF_SIZE
            );
    
    g_music = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_music, LOOP_FOREVER);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    font_texture_id = Utility::load_texture(FONT_FILEPATH);
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    g_menu = new Menu();
    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();
    g_lose   = new Lose();
    g_win    = new Win();
    
    g_levels[0] = g_menu;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;
    g_levels[4] = g_lose;
    g_levels[5] = g_win;
    
    // Start at menu
    switch_to_scene(g_levels[0]);
    
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
//    g_effects->start(FADEIN);
}

void process_input()
{
    if (g_current_scene->get_state().player != nullptr) {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        g_app_status = TERMINATED;
                        break;
                        
                    case SDLK_RETURN:
                        if (g_current_scene == g_menu){
                            switch_to_scene(g_levelA);
                        }
                        break;
                    case SDLK_SPACE:
                        // Jump
                        if (g_current_scene->get_state().player->get_collided_bottom() && g_current_scene->get_state().player != nullptr)
                                                {
                                                    g_current_scene->get_state().player->jump();
                                                    Mix_PlayChannel(-1,  g_current_scene->get_state().jump_sfx, 0);
                                                }
                                                 break;
                    
                    case SDLK_p:
                        pause = !pause;
                        break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    if (!lose && !win){
        if (key_state[SDL_SCANCODE_LEFT])        g_current_scene->get_state().player->move_left();
        else if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->get_state().player->move_right();
        
        if (g_current_scene->get_state().player != nullptr) {
            if (glm::length( g_current_scene->get_state().player->get_movement()) > 1.0f)
                g_current_scene->get_state().player->normalise_movement();
        }
        
    }

    
}

void update()
{
    if (g_current_scene->get_state().player != nullptr) {
        
        
        float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
        float delta_time = ticks - g_previous_ticks;
        g_previous_ticks = ticks;
        
        delta_time += g_accumulator;
        
        if (delta_time < FIXED_TIMESTEP)
        {
            g_accumulator = delta_time;
            return;
        }
        
        while (delta_time >= FIXED_TIMESTEP) {
            g_current_scene->update(FIXED_TIMESTEP);
            g_effects->update(FIXED_TIMESTEP);
            
            
            g_is_colliding_bottom = g_current_scene->get_state().player->get_collided_bottom();
            
            delta_time -= FIXED_TIMESTEP;
        }
        
        g_accumulator = delta_time;
        
        // Prevent the camera from showing anything outside of the "edge" of the level
        if (g_current_scene == g_levelA && g_current_scene->get_state().player->get_position().y < -10.0f)
        {
            switch_to_scene(g_levelB);
            if (lives > 0){
                lives -= 1;
                
            }
            if (lives == 0){
                lose = true;
            }
        }
        if (g_current_scene == g_levelB && g_current_scene->get_state().player->get_position().y < -10.0f) {
            switch_to_scene(g_levelC);
            if (lives > 0){
                lives -= 1;
            }
            if (lives == 0){
                lose = true;
            }
        }
        
        if (g_current_scene == g_levelC && g_current_scene->get_state().player->get_position().y < -10.0f) {
            switch_to_scene(g_levelA);
            if (lives > 0){
                lives -= 1;
            }
            else{
                lose = true;
            }
        }
        if(g_current_scene->get_state().player->check_collision(&g_current_scene->get_state().enemies[0])){
            if (lives > 0){
                lives -= 1;
                Mix_PlayChannel(-1,  g_current_scene->get_state().attack_sfx, 0);
            }
            else{
                lose = true;
            }
        }

        if (g_current_scene == g_levelC && glm::distance(g_current_scene->get_state().player->get_position(), win_pos) <= 1.0f) {
            win = true;
            Mix_PlayChannel(-1,  g_current_scene->get_state().win_sfx, 0);
        }
        
        if (lose == true){
            switch_to_scene(g_lose);
        }
        
        if (win == true){
            switch_to_scene(g_win);
        }
        
        g_view_matrix = glm::mat4(1.0f);
        
        if (!lose && !win){
            if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
                g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.75, 0));
            } else {
                g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
            }
        }
        else{
            g_view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-5, 3.75, 0));
        }
        
    }
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
       
    glClear(GL_COLOR_BUFFER_BIT);
       
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);
       
    g_effects->render();
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    delete g_menu;
    delete g_levelA;
    delete g_levelB;
    delete g_levelC;
    delete g_lose;
    delete g_win;
    delete g_effects;
    
    
    Mix_FreeChunk(g_current_scene->get_state().jump_sfx);
    Mix_FreeChunk(g_current_scene->get_state().win_sfx);
    Mix_FreeChunk(g_current_scene->get_state().attack_sfx);
    Mix_FreeMusic(g_current_scene->get_state().bgm);
}

// ––––– DRIVER GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        if (!pause){
            update();
        }
        
        if (g_current_scene->get_state().next_scene_id >= 0) switch_to_scene(g_levels[g_current_scene->get_state().next_scene_id]);
        
        
        render();
    }

    shutdown();
    return 0;
}
