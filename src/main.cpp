#include <engine/engine.hpp>

int main() {
    engine::Screen screen("name", 800, 400, 1);
    
    engine::Camera* camera0 = screen.createCamera(
        vec2<double>(0, 0),
        vec2<double>(100, 50),
        vec2<double>(0, 0),
        vec2<double>(0.5, 0.5)
    );
    camera0->bg_color = {100, 120, 140};

    engine::Scene* scene = new engine::Scene;
    camera0->bindScene(scene);
    scene->addSpriteObject(
        vec2<double>(10, 10),
        "assets/sprites/image.png",
        vec2<double>(30, 30),
        0
    );

    screen.draw();
    while(1);
}

