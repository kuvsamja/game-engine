#include <engine/engine.hpp>


int main() {
    engine::Screen screen("name", 1600, 800, 1);
    screen.has_anti_aliasing = 1;
    engine::Camera* camera0 = screen.createCamera(
        vec2<double>(0, 0),
        vec2<double>(100, 50),
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
    scene->addSpriteObject(
        "assets/sprites/hornet/colors.png",
        "assets/sprites/hornet/depth_map.png",
        // nullptr,
        vec2<double>(10, 10),
        vec2<double>(30, 40),
        0
    );
    scene->addSpriteObject(
        "assets/sprites/hornet/colors.png",
        "assets/sprites/hornet/depth_map.png",
        vec2<double>(20, 10),
        vec2<double>(30, 40),
        0
    );

    auto point_light = scene->addPointLight(vec3<double>(10, 10, 10), 10, vec3<int>(180, 140, 100), 0.01, 0.01);
    
    engine::GUIGroup* gui_group0 = screen.createGUIGroup(
        vec2<double>(0, 0),
        vec2<double>(1, 1)
    );

    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0, 0), 0.04, 0);
    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.15, 0.1), 0.04, 0);
    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.2, 0.1), 0.04, 0);
    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.25, 0.1), 0.04, 0);
    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.3, 0.1), 0.04, 0);

    int mode = 0;
    while(!screen.shouldClose()){
        for (int k = GLFW_KEY_1; k <= GLFW_KEY_9; ++k) {
            if (glfwGetKey(screen.getWindow(), k) == GLFW_PRESS) {
                mode = k - GLFW_KEY_1;
                std::cout << "Mode set to: " << mode << std::endl;
            }
        }
        
        screen.pollEvents();
        screen.draw(mode);
        screen.swapBuffers();
    }
}