#include "Lose.hpp"
#include "Utility.hpp"


constexpr char FONT_FILEPATH[] = "assets/font1.png";



Lose::~Lose()
{

}

void Lose::initialise()
{
    m_game_state.next_scene_id = -1;
    
    font_texture_id = Utility::load_texture(FONT_FILEPATH);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void Lose::update(float delta_time)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
}

void Lose::render(ShaderProgram *program)
{
    glClear(GL_COLOR_BUFFER_BIT);

    program->set_view_matrix(glm::mat4(1.0f));
    Utility::draw_text(program, font_texture_id, "You", 0.5f, 0.05f, glm::vec3(-1.0f, 1.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Lose", 0.5f, 0.05f, glm::vec3(-1.0f, 0.0f, 0.0f));

}


