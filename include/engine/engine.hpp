#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <vector>
#include <cmath>
#include <cstdint>
#include <iostream>

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
  private:
    SDL_Texture* texture{nullptr};

  public:
    void loadTexture(SDL_Renderer* renderer, const char* path) {
        texture = IMG_LoadTexture(renderer, path);
        if (!texture) {
            SDL_Log("Failed to load image: %s\n", SDL_GetError());
            exit(1);
        }
    }

    SDL_Texture* getTexture() { return texture; }
    /* current sprite */
    vec2<double> sprite_offset;
    vec2<double> sprite_size;
    int z_layer;
    /*
        x, y - position in world
        sprite_w - sprite width
        sprite_h - sprite height
    */
    SpriteObject(double x, double y, double sprite_w, double sprite_h, double z_layer) {
        position = vec2(x, y);
        sprite_size = vec2(sprite_w, sprite_h);
        sprite_offset = vec2(0.0, 0.0);
        this->z_layer = z_layer;
    }

    /*
        position - position in world
        sprite_size - sprite width and height in world
    */
    SpriteObject(vec2<double> position, vec2<double> sprite_size, double z_layer) {
        this->position = position;
        this->sprite_size = sprite_size;
        sprite_offset = vec2(0.0, 0.0);
        this->z_layer = z_layer;

    }

    /*
        x, y - position in world
        sprite_w - sprite width
        sprite_h - sprite height
        sprite_offset_x / sprite_offset_y - sprite offset from object position in world units
    */
    SpriteObject(double x, double y, double sprite_w, double sprite_h, double sprite_offset_x, double sprite_offset_y, double z_layer) {
        position = vec2(x, y);
        sprite_size = vec2(sprite_w, sprite_h);
        sprite_offset = vec2(sprite_offset_x, sprite_offset_y);
        this->z_layer = z_layer;
    }

    /*
        position - position in world
        sprite_size - sprite width and height in world
        sprite_offset - sprite offset from object position in world units
    */
    SpriteObject(vec2<double> position, vec2<double> sprite_size, vec2<double> sprite_offset, double z_layer) {
        this->position = position;
        this->sprite_size = sprite_size;
        this->sprite_offset = sprite_offset;
        this->z_layer = z_layer;
    }

};


class Scene {
  public:
    SDL_Renderer* renderer{nullptr};
    std::vector<SpriteObject*> sprite_objects{};

    SpriteObject* addSpriteObject(vec2<double> position, const char* sprite_path, vec2<double> sprite_size, double z_layer) {
        if(renderer == nullptr) {
            std::cerr << "You must first bind the scene to a camera to upload textures using \"inline void engine::Camera::bindScene(engine::Scene *scene)\"" << std::endl;
            return nullptr;
        }
        SpriteObject* sprite_object = new SpriteObject(position, sprite_size, z_layer);
        sprite_object->loadTexture(renderer, sprite_path);
        sprite_objects.push_back(sprite_object); // TODO: take z_order into account
        return sprite_object;
    }
    

};

class Camera {
  private:
    Scene* scene;
    SDL_Renderer* renderer;
  public:
    /* position in world space */
    vec2<double> position;
    /* size in world space */
    vec2<double> size;
    /* background color */
    SDL_Color bg_color{0, 0, 0, 255};

    Camera(double x, double y, double width, double height, SDL_Renderer* renderer) {
        position = vec2(x, y);
        size = vec2(width, height);
        this->renderer = renderer;
    }

    Camera(vec2<double> position, vec2<double> size, SDL_Renderer* renderer) {
        this->position = position;
        this->size = size;
        this->renderer = renderer;
    }

    void bindScene(Scene* scene) {
        this->scene = scene;
        scene->renderer = this->renderer;
        // TODO: make this load scene from a file and bind textures
    }
    Scene* getScene() { return scene; }

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


class Screen {
  private:
    SDL_Window* window{nullptr};
    SDL_Renderer* renderer{nullptr};
    SDL_Texture* framebuffer{nullptr};
    double window_scale;
    vec2<uint32_t> size;

    // TODO: add gui elements
    std::vector<CameraData> cameras{};


    void init(const char* name) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("SDL_Init failed: %s", SDL_GetError());
            exit(1);
        }
        
        window = SDL_CreateWindow(
            name,
            static_cast<int>(size.x() * window_scale),
            static_cast<int>(size.y() * window_scale),
            SDL_WINDOW_MINIMIZED
            
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

        SDL_SetRenderLogicalPresentation(
            renderer,
            static_cast<int>(size.x()),
            static_cast<int>(size.y()),
            SDL_LOGICAL_PRESENTATION_LETTERBOX
        );

        framebuffer = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            static_cast<int>(size.x()),
            static_cast<int>(size.y())
        );

        SDL_SetTextureScaleMode(framebuffer, SDL_SCALEMODE_NEAREST);

    }

    /* TODO: add multiple overloads */
    /*
        screen_viewport_position - position of the upper left corner of the camera, mapped 0-1 to screenspace
        screen_viewport_size - size of the place displaying the camera, ranges 0-1 as a fraction of the screen size
    */
    void addCamera(Camera* camera, vec2<double> screen_viewport_position, vec2<double> screen_viewport_size) {
        cameras.push_back(
            CameraData{
                camera,
                screen_viewport_position,
                screen_viewport_size
            }
        );
    }

    /*
        screen_viewport_position - position of the upper left corner of the camera, mapped 0-1 to screenspace
        screen_viewport_width / screen_viewport_height - size of the place displaying the camera, ranges 0-1 as a fraction of the screen size
    */
    void addCamera(Camera* camera, double screen_viewport_pos_x, double screen_viewport_pos_y, double screen_viewport_width, double screen_viewport_height) {
        cameras.push_back(
            CameraData{
                camera,
                vec2<double>(screen_viewport_pos_x, screen_viewport_pos_y),
                vec2<double>(screen_viewport_width, screen_viewport_height)
            }
        );
    }


  public:
    int has_anti_aliasing = 1;  

    /*
        name - window name
        width/height - pixel width and height of the window
        window_scale - scales a window up or down by a factor
    */
    Screen(const char* name, uint32_t width, uint32_t height, double window_scale) {
        size.x() = width;
        size.y() = height;
        this->window_scale = window_scale; 
        init(name);
    }

    ~Screen() {
        /* destruct cameras */
        for(auto camera_data : cameras) {
            delete camera_data.camera;
        }

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        SDL_Quit();
    }


    int width() {
        return size.x();
    }
    int height() {
        return size.y();
    }

    /* Creates a camera object, binds it to the screen and returns its pointer */
    Camera* createCamera(vec2<double> position, vec2<double> size, vec2<double> screen_viewport_position, vec2<double> screen_viewport_size) {
        Camera* camera = new Camera(position, size, renderer);
        addCamera(camera, screen_viewport_position, screen_viewport_size);
        return camera;
    }

    /* binds a texture to a sprite_object */
    void bindTexture(SpriteObject* sprite_object, const char* path) {
        sprite_object->loadTexture(renderer, path);
    }

    /* draws all bound cameras */
    void draw() {
        switch(has_anti_aliasing) {
            case 0: SDL_SetTextureScaleMode(framebuffer, SDL_SCALEMODE_NEAREST); break;
            case 1: SDL_SetTextureScaleMode(framebuffer, SDL_SCALEMODE_LINEAR); break;
        }
        SDL_SetRenderTarget(renderer, framebuffer);

        for( const auto &camera_data : cameras ) {
            SDL_Rect viewport {
                static_cast<int>(camera_data.screen_viewport_position.x() * width()),
                static_cast<int>(camera_data.screen_viewport_position.y() * height()),
                static_cast<int>(camera_data.screen_viewport_size.x() * width()),
                static_cast<int>(camera_data.screen_viewport_size.y() * height())
            };
            SDL_SetRenderViewport(renderer, &viewport);

            SDL_SetRenderDrawColor(
                renderer,
                camera_data.camera->bg_color.r,
                camera_data.camera->bg_color.g,
                camera_data.camera->bg_color.b,
                camera_data.camera->bg_color.a
            );
            SDL_FRect clear_rect { 0.0f, 0.0f, static_cast<float>(viewport.w), static_cast<float>(viewport.h) };
            SDL_RenderFillRect(renderer, &clear_rect);

            if (camera_data.camera->getScene() == nullptr) {
                std::cerr << "Please bind a scene to the camera before drawing" << std::endl;
                exit(1);
            }

            for( const auto &sprite_object : camera_data.camera->getScene()->sprite_objects ) {
                /* get things in relation to the camera */
                vec2<double> cam_object_position = camera_data.camera->getPointPosition( sprite_object->position + sprite_object->sprite_offset );
                vec2<double> cam_object_size = vec2<double>(
                    sprite_object->sprite_size.x() / camera_data.camera->size.x(), 
                    sprite_object->sprite_size.y() / camera_data.camera->size.y()
                );
                /* transform to screenspace */
                SDL_FRect sprite_location_data {
                    static_cast<float>(cam_object_position.x() * viewport.w),
                    static_cast<float>(cam_object_position.y() * viewport.h),
                    static_cast<float>(cam_object_size.x() * viewport.w),
                    static_cast<float>(cam_object_size.y() * viewport.h)
                };

                SDL_RenderTexture(renderer, sprite_object->getTexture(), NULL, &sprite_location_data);
            }

            SDL_SetRenderViewport(renderer, NULL);
        }
        SDL_SetRenderTarget(renderer, NULL);

        SDL_RenderTexture(renderer, framebuffer, NULL, NULL);

        SDL_RenderPresent(renderer);
    }
    
};


};




#endif