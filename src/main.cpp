#include "utility/color.h"
#include <chrono>
#include <engine/engine.hpp>
#include <thread>


class AnimatedSprite : public engine::SpriteObject {
  private:
    int animation_timer{0};
    int animation_frame_duration{5}; // in frames
    
    int total_frames{0};
    
    int spritesheet_x{0};
    int spritesheet_y{0};

    int current_sprite{0};
  public:
    AnimatedSprite(std::string path, vec2<double> position, vec2<double> sprite_size, double z_order,
                    int spritesheet_x, int spritesheet_y, int total_frames, Shader* shader)
        : engine::SpriteObject(position, sprite_size, z_order) {
        this->spritesheet_x = spritesheet_x;
        this->spritesheet_y = spritesheet_y;
        this->total_frames = total_frames;
        loadTexture(path.c_str());
        this->shader = shader;
    }

    void draw(const engine::DrawContext& draw_context) override {        
        animation_timer++;
        if (animation_timer % animation_frame_duration == 0) current_sprite = (current_sprite + 1) % total_frames;
        
        shader->use();
        
        shader->setVec2("camera_world_position", draw_context.camera->position);
        shader->setVec2("camera_world_size", draw_context.camera->size);
        /* vertex shader uniforms */
        shader->setVec2("sprite_position", position);
        shader->setVec2("sprite_offset", sprite_offset);
        shader->setVec2("sprite_size", sprite_size);
        shader->setVec2("spritesheet_size", vec2<double>(spritesheet_x, spritesheet_y));
        shader->setInt("current_frame", current_sprite);
        
        // uniform vec2 spritesheet_size
        // uniform int current_frame
        
        // shader->setVec2("frame", vec2<double>(current_frame, total_frames));
        /* fragment shader uniform */
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getSprite());
        shader->setInt("sprite_texture", 0);
    
        glBindVertexArray(draw_context.quad_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    }
          
    
};


int main() {
    engine::Window screen("name", 1600, 800, 1);
    screen.has_anti_aliasing = 1;
    engine::Camera* camera0 = screen.createCamera(
        vec2<double>(0, 0),
        100,
        vec2<double>(0, 0),
        vec2<double>(1, 1)
    );
    camera0->bg_color = {100, 120, 140, 255};

    // engine::Camera* camera1 = screen.createCamera(
    //     vec2<double>(0, 0),
    //     vec2<double>(100, 50),
    //     vec2<double>(0.5, 0),
    //     vec2<double>(0.5, 0.5)
    // );
    // camera1->bg_color = {140, 120, 100, 255};
    
    engine::Scene* scene = new engine::Scene;
    camera0->bindScene(scene);
    // camera1->bindScene(scene);
    auto obj1 = scene->addSpriteObject(
        "assets/sprites/hornet/colors.png",
        vec2<double>(10, 10),
        30,
        1
    );
    auto obj2 = scene->addSpriteObject(
        "assets/sprites/hornet/colors.png",
        vec2<double>(20, 10),
        30,
        2
    );
    auto shader = screen.shader_manager.load(
        "animated-sprite",
        "shaders/draw_animated_sprite/vertex.gl",
        "shaders/draw_animated_sprite/fragment.gl"
    );

    auto animated_sprite = std::make_unique<AnimatedSprite>(
        "assets/sprites/hornet-walk/colors.png",
        vec2<double>(20, 10),
        vec2<double>(30, 40),
        10,
        5,
        2,
        5,
        shader
    );

    auto animated = scene->addSpriteObject(std::move(animated_sprite));
    
    engine::GUIGroup* gui_group0 = screen.createGUIGroup(
        vec2<double>(0, 0),
        vec2<double>(1, 1)
    );

    gui_group0->newImageElement("assets/gui-elements/mask.png", vec2<double>(0, 0), 0.04, 0);
    gui_group0->newImageElement("assets/gui-elements/mask.png", vec2<double>(0.15, 0.1), 0.04, 0);
    gui_group0->newImageElement("assets/gui-elements/mask.png", vec2<double>(0.2, 0.1), 0.04, 0);
    gui_group0->newImageElement("assets/gui-elements/mask.png", vec2<double>(0.25, 0.1), 0.04, 0);
    gui_group0->newImageElement("assets/gui-elements/mask-transparent.png", vec2<double>(0.3, 0.1), 0.04, 0);

    auto comic_relief_font = std::make_unique<engine::Font>("assets/fonts/ComicRelief-Regular.ttf", 40);
    gui_group0->newTextElement(comic_relief_font.get(), 
                                "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras eu congue eros. Morbi commodo nisi accumsan consectetur iaculis. Cras eu luctus magna, vitae aliquet est. Cras scelerisque volutpat accumsan. Suspendisse potenti. Pellentesque a purus sed eros efficitur sodales. Cras sed vulputate ante, nec laoreet turpis. Fusce quis suscipit orci. Donec vel porttitor sapien. Suspendisse potenti. Ut vel ipsum sem. ",
                                vec2<double>(0.3, 0.5), 0.2, 0, RGBAColor{255, 255, 255, 100});

    gui_group0->newTextElement(comic_relief_font.get(), "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras eu congue eros. Morbi commodo",
        vec2<double>(0.5, 0.5), 0.5, 1, RGBAColor{255, 0, 0, 255});
    
    while(!screen.shouldClose()){        
        screen.pollEvents();
        if(screen.buttonDown(GLFW_KEY_W)) animated->position.y()++;
        if(screen.buttonDown(GLFW_KEY_S)) animated->position.y()--;
        if(screen.buttonDown(GLFW_KEY_D)) animated->position.x()++;
        if(screen.buttonDown(GLFW_KEY_A)) animated->position.x()--;
        screen.draw();
        screen.swapBuffers();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

}