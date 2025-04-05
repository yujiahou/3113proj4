#include "Scene.hpp"

class Menu : public Scene {
public:
    int ENEMY_COUNT = 1;
    
    ~Menu();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    
    GLuint font_texture_id;
};

