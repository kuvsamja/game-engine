#include <chrono>
#include <engine/engine.hpp>
#include <thread>


int main() {
    engine::Screen screen("name", 1600, 800, 1);
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
    
    engine::GUIGroup* gui_group0 = screen.createGUIGroup(
        vec2<double>(0, 0),
        vec2<double>(1, 1)
    );

    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0, 0), 0.04, 0);
    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.15, 0.1), 0.04, 0);
    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.2, 0.1), 0.04, 0);
    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.25, 0.1), 0.04, 0);
    gui_group0->newElement("assets/gui-elements/mask.png", vec2<double>(0.3, 0.1), 0.04, 0);

    int timer = 0;
    while(!screen.shouldClose()){
        timer++;
        
        screen.pollEvents();
        screen.draw();
        screen.swapBuffers();
        if (timer % 2 == 0) scene->changeSpriteZ(obj1, obj2->z_layer+1);
        else scene->changeSpriteZ(obj2, obj1->z_layer+1);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}