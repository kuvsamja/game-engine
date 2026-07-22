#include <engine/engine.hpp>

#include <memory>

int main() {
    engine::Screen screen("name", 800, 400, 1);
    engine::Camera camera0(0, 0, 100, 50);
    screen.addCamera(&camera0, 0, 0, 1, 1);
    
    engine::SpriteObject* obj1 = new engine::SpriteObject(10, 10, 30, 30, 0, 0);

    camera0.sprite_objects.push_back(obj1);

    screen.init("nm");
    screen.bindTexture(obj1, "assets/sprites/image.png");

    screen.draw();
    while(1);
}

