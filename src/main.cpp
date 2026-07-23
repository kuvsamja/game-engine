#include "../include/engine/engine.hpp"
#include <thread>
#include <chrono>


int main() {
    engine::Screen screen("name", 800, 400, 1);
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
        vec2<double>(10, 10),
        "assets/sprites/image.png",
        vec2<double>(30, 40),
        1
    );
    scene->addSpriteObject(
        vec2<double>(20, 10),
        "assets/sprites/image.png",
        vec2<double>(30, 40),
        0
    );

    engine::GUIGroup* gui_group0 = screen.createGUIGroup(
        vec2<double>(0, 0),
        vec2<double>(1, 1)
    );

    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.1, 0.1), 0.04, 0);
    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.15, 0.1), 0.04, 0);
    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.2, 0.1), 0.04, 0);
    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.25, 0.1), 0.04, 0);
    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.3, 0.1), 0.04, 0);

    screen.draw();
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

