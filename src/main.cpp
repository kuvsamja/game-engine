#include "../include/engine/engine.hpp"
#include <thread>
#include <chrono>

int main() {
    engine::Screen screen("name", 800, 400, 1);
    screen.has_anti_aliasing = 0;
    engine::Camera* camera0 = screen.createCamera(
        vec2<double>(0, 0),
        vec2<double>(100, 50),
        vec2<double>(0, 0),
        vec2<double>(0.5, 0.5)
    );
    camera0->bg_color = {100, 120, 140, 255};

    engine::Scene* scene = new engine::Scene;
    camera0->bindScene(scene);
    scene->addSpriteObject(
        vec2<double>(10, 10),
        "assets/sprites/image.png",
        vec2<double>(30, 30),
        -1
    );
    scene->addSpriteObject(
        vec2<double>(20, 10),
        "assets/sprites/image.png",
        vec2<double>(30, 30),
        0
    );

    screen.draw();
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

