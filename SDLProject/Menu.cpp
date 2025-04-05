#include "Menu.hpp"
#include "Utility.hpp"


constexpr char FONT_FILEPATH[] = "assets/font1.png";



Menu::~Menu()
{

}

void Menu::initialise()
{
    m_game_state.next_scene_id = -1;
    
    font_texture_id = Utility::load_texture(FONT_FILEPATH);

}

void Menu::update(float delta_time)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_RETURN:
                        m_game_state.next_scene_id = 1;
                        break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
}

void Menu::render(ShaderProgram *program)
{
    Utility::draw_text(program, font_texture_id, "Press", 0.5f, 0.05f, glm::vec3(-3.0f, 2.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "enter", 0.5f, 0.05f, glm::vec3(-3.0f, 1.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "to", 0.5f, 0.05f, glm::vec3(-3.0f, 0.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "start", 0.5f, 0.05f, glm::vec3(-3.0f, -1.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "rise of the AI", 0.5f, 0.05f, glm::vec3(-3.0f, -2.0f, 0.0f));
}

