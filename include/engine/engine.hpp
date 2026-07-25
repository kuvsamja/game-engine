#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <cstdint>
#include <iostream>

#include <vec3.hpp>
#include <vec2.hpp>
#include <color.h>
#include <SortedVector.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace engine
{
/*
    Loads an image from a path into the GLuint
 */
void loadGLTexture(GLuint* texture, const char* path, int* width, int* height, int* channels) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture); 
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load(path, width, height, channels, 4);
    if (data == NULL) {
        std::cerr << "ERROR: failed to load image data!" << std::endl;
        exit(1);
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

class Transform {
  public:
    /* position in world space */
    vec2<double> position;
};

class SpriteObject : public Transform {
  private:
    /* current sprite */
    
    GLuint sprite{0};
    GLuint normal_map{0};

  public:
    vec2<double> sprite_offset;
    vec2<double> sprite_size;
    double z_layer;
    bool ignore_light{false};
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
    SpriteObject(vec2<double> position, vec2<double> sprite_size, double z_layer) { // TODO: PUT SPRITE OFFSET INTO THE CONSTRUCTOR
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
        z_layer - sprite depth ordering, smaller z means sprite is drawn below others
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
        z_layer - sprite depth ordering, smaller z means sprite is drawn below others
    */
    SpriteObject(vec2<double> position, vec2<double> sprite_size, vec2<double> sprite_offset, double z_layer) {
        this->position = position;
        this->sprite_size = sprite_size;
        this->sprite_offset = sprite_offset;
        this->z_layer = z_layer;
    }
    ~SpriteObject() {
        if (sprite) glDeleteTextures(1, &sprite);
        if (normal_map) glDeleteTextures(1, &normal_map);
    }

    /*
        normal map image and sprite image must be the same resulution TODO: make them not need to be same resolution 
        Load an image as the sprite
    */
    void loadTexture(const char* sprite_image_path, const char* normal_map_path) {
        int sprite_width, sprite_height, sprite_channels;
        loadGLTexture(&sprite, sprite_image_path, &sprite_width, &sprite_height, &sprite_channels);
        if (!sprite) {
            std::cerr << "Failed to load sprite texture from the path: \"" << sprite_image_path << "\"" << std::endl;
            exit(1);
        }
        int normal_map_width, normal_map_height, normal_map_channels;
        loadGLTexture(&normal_map, normal_map_path, &normal_map_width, &normal_map_height, &normal_map_channels);
        if (!normal_map) {
            std::cerr << "Failed to load normal_map texture from the path: \"" << normal_map_path << "\"" << std::endl;
            exit(1);
        }
        
    }

    GLuint getSprite() { return sprite; }
    GLuint getNormalMap() { return normal_map; }
    

};



class Scene {
  private:
    static inline bool compareZ(SpriteObject* const& s1, SpriteObject* const& s2) {
        return s1->z_layer > s2->z_layer;
    }

  public:
    SortedVector<SpriteObject*> sprite_objects{compareZ};

    ~Scene() {
        for(auto sprite_object : sprite_objects) {
            delete sprite_object;
        }
    }

    /*
        position - position in worldspace
        sprite_path - file path to the image of the sprite
        sprite_size - sprite width and height in worldspace
        z_layer - sprite depth ordering, smaller z means sprite is drawn below others
        creates a SpriteObject and pushes it into the Scene
    */
    SpriteObject* addSpriteObject(const char* sprite_path, const char* normal_map_path, vec2<double> position, vec2<double> sprite_size, double z_layer) {
        SpriteObject* sprite_object = new SpriteObject(position, sprite_size, z_layer);
        sprite_object->loadTexture(sprite_path, normal_map_path);
        sprite_objects.insert(sprite_object);
        return sprite_object;
    }



};

class Camera {
  private:
    Scene* scene;
  public:
    /* position in world space */
    vec2<double> position;
    /* size in world space */
    vec2<double> size;
    /* background color */
    RGBAColor bg_color{0, 0, 0, 255};

    /*
        x, y - cameras upper left corner position in worldspace
        width, height - width and height of the camera viewport in world units
        renderer - sdl renderer, should be set by engine::Screen
        *shouldnt be called directly, use engine::Screen::createCamera
    */
    Camera(double x, double y, double width, double height) {
        position = vec2(x, y);
        size = vec2(width, height);
    }

    /*
        position - cameras upper left corner position in worldspace
        size - width, size.x(), and height, size.y(), of the camera viewport in world units
        renderer - sdl renderer, should be set by engine::Screen
        *shouldnt be called directly, use engine::Screen::createCamera
    */
    Camera(vec2<double> position, vec2<double> size) {
        this->position = position;
        this->size = size;
    }

    /*
        Sets scenes SDL_Renderer to the cameras SDL_Renderer and binds the scene to the camera
    */
    void bindScene(Scene* scene) {
        this->scene = scene;
        // TODO: make this load scene from a file and bind textures
    }
    Scene* getScene() { return scene; }

    /*
        Returns the screen position of a point, mapped 0 to 1, values (-inf, 0)U(1, +inf) mean object is out of the camera sight
    */
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

struct GUIElement {
  public:
    GLuint texture;
    vec2<double> position;
    double width;
    double aspect_ratio;
    double z_order;
    ~GUIElement() {
        glDeleteTextures(1, &texture);
    }
};

class GUIGroup {
  private:
    static inline bool compareZ(GUIElement* const& a, GUIElement* const& b) {
        return a->z_order > b->z_order;
    }
  public:
    SortedVector<GUIElement*> gui_texture_elements{compareZ};
    vec2<double> viewport_position;
    vec2<double> viewport_size;

    GUIGroup(vec2<double> viewport_position, vec2<double> viewport_size) {
        this->viewport_position = viewport_position;
        this->viewport_size = viewport_size;
    }

    ~GUIGroup() {
        for(auto element : gui_texture_elements) delete element;
    }


    /*
        path - path to the texture image file
        position - position in the GUIGroup viewport, mapped 0-1
        width - width relative to the viewport width, scaled 0-1
        z_order - decides which elements are drawn on top of others
        note: gui aspect ratio doesnt depend on screen or viewport aspect ratios 
     */
    GUIElement* newElement(const char* path, vec2<double> position, double width, double z_order) { // TODO: make opengl load the texture
        GUIElement* element = new GUIElement;
        element->position = position;
        element->z_order = z_order;

        int image_width;
        int image_height;
        int channels;
        loadGLTexture(&(element->texture), path, &image_width, &image_height, &channels);
        double aspect_ratio = static_cast<double>(image_height) / image_width;
        element->width = width;
        element->aspect_ratio = aspect_ratio;
        gui_texture_elements.insert(element);

        return element;
    }
    // TODO: mabye add custom aspect ratio gui
};

class Screen {
  private:
    GLFWwindow* window{nullptr};
    double window_scale;
    vec2<uint32_t> size;

    std::vector<CameraData> cameras{};
    std::vector<GUIGroup*> gui_groups{};
    /*
        Initializes OpenGL window and stuff
    */
    void init(const char* name) {
        if (!glfwInit()) {
            std::cerr << "glfwInit failed" << std::endl;
            exit(1);
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
        window = glfwCreateWindow(
            static_cast<int>(size.x() * window_scale),
            static_cast<int>(size.y() * window_scale),
            name,
            nullptr,
            nullptr
        );
        if (!window) {
            std::cerr << "glfwCreateWindow failed" << std::endl;
            glfwTerminate();
            exit(1);
        }
        glfwMakeContextCurrent(window);
        
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "gladLoadGLLoader failed" << std::endl;
            exit(1);
        }
        glViewport(0, 0, static_cast<int>(size.x() * window_scale), static_cast<int>(size.y() * window_scale));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

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
    Screen(const char* name, uint32_t width, uint32_t height, double window_scale) { // TODO: the quad thing opengl
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
        for(auto gui_group : gui_groups) {
            delete gui_group;
        }
        glfwDestroyWindow(window);
        glfwTerminate();
    }


    int width() {
        return size.x();
    }
    int height() {
        return size.y();
    }

    /*
        Creates a camera object on the heap, binds it to the screen and returns pointer to itself
        position - camera position in worldspace
        size - camera viewport width and height in world units
        screen_viewport_position - position of the upper left corner of the camera, mapped 0-1 to screenspace
        screen_viewport_size - size of the place displaying the camera, ranges 0-1 as a fraction of the screen size
    */
    Camera* createCamera(vec2<double> position, vec2<double> size, vec2<double> screen_viewport_position, vec2<double> screen_viewport_size) {
        Camera* camera = new Camera(position, size);
        addCamera(camera, screen_viewport_position, screen_viewport_size);
        return camera;
    }

    /*
        viewport_postion - position of the upper left corner of the group on the screen, mapped 0-1
        viewport_size - size of the viewport relative to the screen size, mapped 0-1
     */
    GUIGroup* createGUIGroup(vec2<double> viewport_postion, vec2<double> viewport_size) {
        GUIGroup* gui_group = new GUIGroup(viewport_postion, viewport_size);
        gui_groups.push_back(gui_group);

        return gui_group;
    }

    /*
        Binds a texture to a sprite_object
        *shouldnt really be used*
    */
    void bindTexture(SpriteObject* sprite_object, const char* sprite_image_path, const char* normal_map_path) {
        sprite_object->loadTexture(sprite_image_path, normal_map_path);
    }

    /*
        Draws the scenes bound to cameras on the screen
    */
    void drawSprites() { // TODO: Migrate to shaders with opengl
        for( const auto &camera_data : cameras ) {
            SDL_Rect viewport {
                static_cast<int>(camera_data.screen_viewport_position.x() * width()),
                static_cast<int>(camera_data.screen_viewport_position.y() * height()),
                static_cast<int>(camera_data.screen_viewport_size.x() * width()),
                static_cast<int>(camera_data.screen_viewport_size.y() * height())
            };

            
            // SDL_SetRenderViewport(renderer, &viewport);

            // SDL_SetRenderDrawColor(
            //     renderer,
            //     camera_data.camera->bg_color.r,
            //     camera_data.camera->bg_color.g,
            //     camera_data.camera->bg_color.b,
            //     camera_data.camera->bg_color.a
            // );
            // SDL_FRect clear_rect { 0.0f, 0.0f, static_cast<float>(viewport.w), static_cast<float>(viewport.h) };
            // SDL_RenderFillRect(renderer, &clear_rect);

            if (camera_data.camera->getScene() == nullptr) {
                std::cerr << "Please bind a scene to the camera before drawing" << std::endl;
                exit(1);
            }

            
            for( const auto &sprite_object : camera_data.camera->getScene()->sprite_objects ) {
                /* get things in relation to the camera */
                vec2<double> cam_object_position = camera_data.camera->getPointPosition( sprite_object->position + sprite_object->sprite_offset ); // TODO: change name to cam_sprite_position  
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
    }

    void drawGUI() {
        SDL_SetRenderTarget(renderer, framebuffer);

        for(auto gui_group : gui_groups) {
            SDL_Rect viewport {
                static_cast<int>(gui_group->viewport_position.x() * width()),
                static_cast<int>(gui_group->viewport_position.y() * height()),
                static_cast<int>(gui_group->viewport_size.x() * width()),
                static_cast<int>(gui_group->viewport_size.y() * height())
            };
            SDL_SetRenderViewport(renderer, &viewport);
            
            for(auto gui_texture_element : gui_group->gui_texture_elements) {
                float pixel_width = gui_texture_element->width * viewport.w;
                float pixel_height = pixel_width * gui_texture_element->aspect_ratio;

                SDL_FRect element_location_data {
                    static_cast<float>(gui_texture_element->position.x() * viewport.w),
                    static_cast<float>(gui_texture_element->position.y() * viewport.h),
                    pixel_width,
                    pixel_height
                };

                SDL_RenderTexture(renderer, gui_texture_element->texture, NULL, &element_location_data);
                
            }
            SDL_SetRenderViewport(renderer, NULL);
        }
    }

    void draw() {
        
        switch(has_anti_aliasing) {
            case 0: SDL_SetTextureScaleMode(framebuffer, SDL_SCALEMODE_NEAREST); break;
            case 1: SDL_SetTextureScaleMode(framebuffer, SDL_SCALEMODE_LINEAR); break;
        }

        
        drawSprites();
        drawGUI();

        
        SDL_SetRenderTarget(renderer, NULL);

        SDL_RenderTexture(renderer, framebuffer, NULL, NULL);

        SDL_RenderPresent(renderer);
    }
};


};


#endif