#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <vector>
#include <cmath>
#include <cstdint>

#include <engine/vec2.hpp>

namespace engine
{

class Transform {
  public:
    /* position in world space */
    vec2<double> position;
    /* speed in world space */
    vec2<double> speed;
};

class SpriteObject : public Transform {
  public:
    /* current sprite */
    SDL_Texture* texture;
    vec2<double> sprite_offset;
    vec2<double> sprite_size;

    /*
        x, y - position in world
        sprite_w - sprite width
        sprite_h - sprite height
    */
    SpriteObject(double x, double y, double sprite_w, double sprite_h) {
        position = vec2(x, y);
        sprite_size = vec2(sprite_w, sprite_h);
        sprite_offset = vec2(0.0, 0.0);
    }

    /*
        position - position in world
        sprite_size - sprite width and height in world
    */
    SpriteObject(vec2<double> position, vec2<double> sprite_size) {
        this->position = position;
        this->sprite_size = sprite_size;
        sprite_offset = vec2(0.0, 0.0);

    }

    /*
        x, y - position in world
        sprite_w - sprite width
        sprite_h - sprite height
        sprite_offset_x / sprite_offset_y - sprite offset from object position in world units
    */
    SpriteObject(double x, double y, double sprite_w, double sprite_h, double sprite_offset_x, double sprite_offset_y) {
        position = vec2(x, y);
        sprite_size = vec2(sprite_w, sprite_h);
        sprite_offset = vec2(sprite_offset_x, sprite_offset_y);
    }

    /*
        position - position in world
        sprite_size - sprite width and height in world
        sprite_offset - sprite offset from object position in world units
    */
    SpriteObject(vec2<double> pos, vec2<double> sprite_size, vec2<double> sprite_offset) {
        this->position = position;
        this->sprite_size = sprite_size;
        this->sprite_offset = sprite_offset;
    }

    void loadTexture(SDL_Renderer* renderer, const char* path) {
        texture = IMG_LoadTexture(renderer, path);
        if (!texture) {
            SDL_Log("Failed to load image: %s\n", SDL_GetError());
            exit(1);
        }
    }
};


class Camera {
  public:
    /* position in world space */
    vec2<double> position;
    /* size in world space */
    vec2<double> size;
    /* background color */
    // SDL_Color color

    std::vector<SpriteObject*> sprite_objects;

    Camera(double x, double y, double width, double height) {
        position = vec2(x, y);
        size = vec2(width, height);
    }

    Camera(vec2<double> position, vec2<double> size) {
        this->position = position;
        this->size = size;
    }

    /* Returns the screen position of a point, mapped 0 to 1*/
    vec2<double> getPointPosition(vec2<double> point) {
        vec2<double> translated_point = point - position;
        vec2<double> screen_point_position = vec2<double>(translated_point.x() / size.x(), translated_point.y() / size.y());
        return screen_point_position;
    }
};

struct CameraData {
    Camera* camera;
    /* space on screen, mapped 0 to 1 */
    vec2<double> screen_viewport_position;
    vec2<double> screen_viewport_size;
};


// mabye add a texture manager owned by screen

class Screen {
  private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    double window_scale;

    vec2<uint32_t> size;

    // TODO: add gui elements
    std::vector<CameraData> cameras{};

  public:
    
    Screen(const char* name, uint32_t width, uint32_t height, double window_scale) {
        size.x() = width;
        size.y() = height;
        this->window_scale = window_scale; 
        init(name);
    }

    ~Screen() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        SDL_Quit();
    }

    void init(const char* name) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("SDL_Init failed: %s", SDL_GetError());
            exit(1);
        }

        window = SDL_CreateWindow(
            name,
            static_cast<int>(size.x() * window_scale),
            static_cast<int>(size.y() * window_scale),
            0
        );
        if (window == NULL) {
            SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
            exit(1);
        }

        renderer = SDL_CreateRenderer(window, NULL);
        if (renderer == NULL) {
            SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
            exit(1);
        }

    }

    int width() {
        return size.x();
    }
    int height() {
        return size.y();
    }

    void addCamera(Camera* camera, vec2<double> screen_viewport_position, vec2<double> screen_viewport_size) {
        cameras.push_back(
            CameraData{
                camera,
                screen_viewport_position,
                screen_viewport_size
            }
        );
    }

    void addCamera(Camera* camera, double screen_viewport_pos_x, double screen_viewport_pos_y, double screen_viewport_width, double screen_viewport_height) {
        cameras.push_back(
            CameraData{
                camera,
                vec2<double>(screen_viewport_pos_x, screen_viewport_pos_y),
                vec2<double>(screen_viewport_width, screen_viewport_height)
            }
        );
    }

    void bindTexture(SpriteObject* sprite_object, const char* path) {
        sprite_object->loadTexture(renderer, path);
    }

    void draw() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for( const auto &camera_data : cameras ) {
            SDL_FRect viewport {
                static_cast<float>(camera_data.screen_viewport_position.x() * width()),
                static_cast<float>(camera_data.screen_viewport_position.y() * height()),
                static_cast<float>(camera_data.screen_viewport_size.x() * width()),
                static_cast<float>(camera_data.screen_viewport_size.y() * height())
            };

            for( const auto &sprite_object : camera_data.camera->sprite_objects ) {
                /* get things in relation to the camera */
                vec2<double> cam_object_position = camera_data.camera->getPointPosition( sprite_object->position + sprite_object->sprite_offset );
                vec2<double> cam_object_size = vec2<double>(
                    sprite_object->sprite_size.x() / camera_data.camera->size.x(), 
                    sprite_object->sprite_size.y() / camera_data.camera->size.y()
                );
                /* transform to screenspace */
                SDL_FRect sprite_location_data {
                    static_cast<float>(cam_object_position.x() * camera_data.screen_viewport_size.x() * width()),
                    static_cast<float>(cam_object_position.y() * camera_data.screen_viewport_size.y() * height()),
                    static_cast<float>(cam_object_size.x() * camera_data.screen_viewport_size.x() * width()),
                    static_cast<float>(cam_object_size.y() * camera_data.screen_viewport_size.y() * height())
                };

                SDL_RenderTexture(renderer, sprite_object->texture, NULL, &sprite_location_data);
            }

            SDL_SetRenderViewport(renderer, NULL);
        }

        SDL_RenderPresent(renderer);
    }
    
};


};




#endif