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
#include <shader.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace engine
{
/*
    Loads an image from a path into the GLuint
 */
inline void loadGLTexture(GLuint* texture, const char* path, int* width, int* height, int* channels) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load(path, width, height, channels, 4);
    if (data == NULL) {
        std::cerr << "ERROR: failed to load image data" << std::endl;
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
    bool has_normal_map{true};
    /*
        x, y - position in world
        sprite_w - sprite width
        sprite_h - sprite height
    */
    SpriteObject(double x, double y, double sprite_w, double sprite_h, double z_layer) {
        position = vec2<double>(x, y);
        sprite_size = vec2<double>(sprite_w, sprite_h);
        sprite_offset = vec2<double>(0.0, 0.0);
        this->z_layer = z_layer;
    }

    /*
        position - position in world
        sprite_size - sprite width and height in world
    */
    SpriteObject(vec2<double> position, vec2<double> sprite_size, double z_layer) { // TODO: PUT SPRITE OFFSET INTO THE CONSTRUCTOR
        this->position = position;
        this->sprite_size = sprite_size;
        sprite_offset = vec2<double>(0.0, 0.0);
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
        position = vec2<double>(x, y);
        sprite_size = vec2<double>(sprite_w, sprite_h);
        sprite_offset = vec2<double>(sprite_offset_x, sprite_offset_y);
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
        pass nullptr to normal_map_path to disable normal map lighting for this object
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
        if(normal_map_path == nullptr) {
            has_normal_map = false;
            return;
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

class PointLight : public Transform { // TODO: add max effect distance for performance
  public:
    double intenisty;
    double z;
    RGBAColor color;
    double linear_dropoff;
    double quadratic_dropoff;

    PointLight(vec3<double> position, double intensity, vec3<int> color, double linear_dropoff, double quadratic_dropoff) {
        this->position = vec2<double>(position.x(), position.y());
        z = position.z();
        this->intenisty = intensity;
        this->color.r = color.x();
        this->color.g = color.y();
        this->color.b = color.z();
        this->color.a = 255;
        this->linear_dropoff = linear_dropoff;
        this->quadratic_dropoff = quadratic_dropoff;
    }

};

class Scene {
  private:
    static inline bool compareZ(SpriteObject* const& s1, SpriteObject* const& s2) {
        return s1->z_layer > s2->z_layer;
    }

  public:
    SortedVector<SpriteObject*> sprite_objects{compareZ};
    std::vector<PointLight*> point_lights{};

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
    // TODO: write function description
    PointLight* addPointLight(vec3<double> position, double intensity, vec3<int> color, double linear_dropoff, double quadratic_dropoff) {
        PointLight* point_light = new PointLight(
            position,
            intensity,
            color,
            linear_dropoff,
            quadratic_dropoff
        );
        point_lights.push_back(point_light);
        return point_light;
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
        position = vec2<double>(x, y);
        size = vec2<double>(width, height);
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

class CameraData {
  private:
    void FBOSetup(GLuint& fbo, GLuint& texture, int pixel_width, int pixel_height, GLuint internal_format, GLenum format, GLenum type) {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, pixel_width, pixel_height, 0, format, type, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer not complete" << std::endl;
            exit(1);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    void GBufferSetup(GLuint& fbo, GLuint& color_tex, GLuint& normal_tex, GLuint& depth_tex,
                       GLuint& ignore_light_tex, int pixel_width, int pixel_height) {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        auto attach = [&](GLuint& tex, GLenum attachment, GLint internal_format, GLenum format, GLenum type) {
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, pixel_width, pixel_height, 0, format, type, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex, 0);
        };

        attach(color_tex,        GL_COLOR_ATTACHMENT0, GL_RGBA8,   GL_RGBA, GL_UNSIGNED_BYTE);
        attach(normal_tex,       GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        attach(depth_tex,        GL_COLOR_ATTACHMENT2, GL_R32F,    GL_RED,  GL_FLOAT);
        attach(ignore_light_tex, GL_COLOR_ATTACHMENT3, GL_R8,      GL_RED,  GL_UNSIGNED_BYTE);

        GLenum draw_buffers[] = {
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
        };
        glDrawBuffers(4, draw_buffers);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Gbuffer FBO not complete" << std::endl;
            exit(1);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

  public:
    Camera* camera;

    vec2<double> screen_viewport_position;
    vec2<double> screen_viewport_size;

    GLuint framebuffer_fbo;
    GLuint framebuffer_texture; // vec4 uint8
    GLuint light_fbo;
    GLuint light_texture;  // vec4 float full range

    GLuint gbuffer_fbo;
    GLuint color_texture;  // vec4 float 0-1
    GLuint normal_map_texture;  // vec4 float 0-1
    GLuint depth_texture;  // float full range
    GLuint ignore_light_texture;  // bool

    int pixel_width, pixel_height;


    CameraData(Camera* camera, int screen_pixel_width, int screen_pixel_height,
        vec2<double> screen_viewport_position, vec2<double> screen_viewport_size){
        this->camera = camera;
        this->screen_viewport_position = screen_viewport_position;
        this->screen_viewport_size = screen_viewport_size;

        int pixel_width = screen_pixel_width * screen_viewport_size.x();
        int pixel_height = screen_pixel_height * screen_viewport_size.y();

        FBOSetup(framebuffer_fbo, framebuffer_texture, pixel_width, pixel_height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
        FBOSetup(light_fbo, light_texture, pixel_width, pixel_height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        GBufferSetup(gbuffer_fbo, color_texture, normal_map_texture, depth_texture, ignore_light_texture, pixel_width, pixel_height);
    }

    ~CameraData() {
        glDeleteTextures(1, &framebuffer_texture);
        glDeleteFramebuffers(1, &framebuffer_fbo);
        glDeleteTextures(1, &color_texture);
        glDeleteTextures(1, &normal_map_texture);
        glDeleteTextures(1, &depth_texture);
        glDeleteTextures(1, &ignore_light_texture);
        glDeleteTextures(1, &light_texture);
        glDeleteFramebuffers(1, &light_fbo);
        glDeleteFramebuffers(1, &gbuffer_fbo);
    }

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
    GUIElement* newElement(const char* path, vec2<double> position, double width, double z_order) {
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

    std::vector<CameraData*> cameras{};
    std::vector<GUIGroup*> gui_groups{};

    GLuint quad_vao{0};
    GLuint quad_vbo{0};

    Shader* sprite_draw_shader{nullptr};
    Shader* gui_draw_shader{nullptr};
    Shader* gbuffer_shader{nullptr};
    Shader* light_buffer_shader{nullptr};
    Shader* final_light_pass_shader{nullptr};

    /*
        Initializes GLFW window and the OpenGL context
    */
    void init(const char* name) {
        if (!glfwInit()) {
            std::cerr << "glfwInit failed" << std::endl;
            exit(1);
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        #ifdef __APPLE__
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        #endif
        window = glfwCreateWindow(
            static_cast<int>(size.x()),
            static_cast<int>(size.y()),
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

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
                                  GLsizei length, const char* message, const void* userParam) {
            GLint current_program;
            glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
            std::cerr << "[GL DEBUG] (program=" << current_program << ") " << message << std::endl;
        }, nullptr);

        glViewport(0, 0, static_cast<int>(size.x()), static_cast<int>(size.y()));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    }

    void createQuad() {
        float quad_verts[] = {
            // x,    y,    u,    v
            0.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
        };

        glGenVertexArrays(1, &quad_vao);
        glGenBuffers(1, &quad_vbo);

        glBindVertexArray(quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void makeShaders() {
        sprite_draw_shader = new Shader(
            "shaders/sprite_drawing/vertex.gl",
            "shaders/sprite_drawing/fragment.gl"
        );
        gui_draw_shader = new Shader(
            "shaders/gui_drawing/vertex.gl",
            "shaders/gui_drawing/fragment.gl"
        );
        gbuffer_shader = new Shader(
            "shaders/gbuffer/vertex.gl",
            "shaders/gbuffer/fragment.gl"
        );
        light_buffer_shader = new Shader(
            "shaders/light_buffer/vertex.gl",
            "shaders/light_buffer/fragment.gl"
        );
        final_light_pass_shader = new Shader(
            "shaders/final_light_pass/vertex.gl",
            "shaders/final_light_pass/fragment.gl"
        );
    }

    /*
        screen_viewport_position - position of the upper left corner of the camera, mapped 0-1 to screenspace
        screen_viewport_size - size of the place displaying the camera, ranges 0-1 as a fraction of the screen size
    */
    void addCamera(Camera* camera, vec2<double> screen_viewport_position, vec2<double> screen_viewport_size) {
        CameraData* camera_data = new CameraData(
            camera,
            width(),
            height(),
            screen_viewport_position,
            screen_viewport_size
        );
        cameras.push_back(camera_data);
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
        makeShaders();
        createQuad();
    }

    ~Screen() {
        /* destruct cameras */
        for(auto camera_data : cameras) {
            delete camera_data;
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
        Swapps glfw buffers to update the screen
    */
    void swapBuffers() {
        glfwSwapBuffers(window);
    }
    /*
        Polls window events
    */
    void pollEvents() {
        glfwPollEvents();
    }
    /*
        Returns true when user clicks X button
    */
    bool shouldClose() {
        return glfwWindowShouldClose(window);
    }
    GLFWwindow * getWindow() { return window; }

    void clearGBuffer(CameraData* camera_data) {
        glBindFramebuffer(GL_FRAMEBUFFER, camera_data->gbuffer_fbo);
        float color_clear[4] = {
            camera_data->camera->bg_color.r / 255.0f,
            camera_data->camera->bg_color.g / 255.0f,
            camera_data->camera->bg_color.b / 255.0f,
            camera_data->camera->bg_color.a / 255.0f
        };
        glClearBufferfv(GL_COLOR, 0, color_clear);

        float normal_map_clear[4] = {0, 0, 1, 0};
        glClearBufferfv(GL_COLOR, 1, normal_map_clear);

        float depth_map_clear[4] = {9999, 0, 0, 0};
        glClearBufferfv(GL_COLOR, 2, depth_map_clear);

        float ignore_light_clear[4] = {1, 0, 0, 0};
        glClearBufferfv(GL_COLOR, 3, ignore_light_clear);

    }

    void drawGBuffer(CameraData* camera_data) {
        glDisable(GL_BLEND);
        glBindFramebuffer(GL_FRAMEBUFFER, camera_data->gbuffer_fbo);
        int pixel_width = static_cast<int>(width() * camera_data->screen_viewport_size.x());
        int pixel_height = static_cast<int>(height() * camera_data->screen_viewport_size.y());
        glViewport(0, 0, pixel_width, pixel_height);

        clearGBuffer(camera_data);
        gbuffer_shader->use();

        gbuffer_shader->setVec2(
            "camera_world_position",
            camera_data->camera->position
        );
        gbuffer_shader->setVec2(
            "camera_world_size",
            camera_data->camera->size
        );

        for( const auto &sprite_object : camera_data->camera->getScene()->sprite_objects ) {
            /* vertex shader uniforms */
            gbuffer_shader->setVec2(
                "sprite_position",
                sprite_object->position
            );
            gbuffer_shader->setVec2(
                "sprite_offset",
                sprite_object->sprite_offset
            );
            gbuffer_shader->setVec2(
                "sprite_size",
                sprite_object->sprite_size
            );
            /* fragment shader uniform */
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sprite_object->getSprite());
            gbuffer_shader->setInt("sprite_texture", 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, sprite_object->getNormalMap());
            gbuffer_shader->setInt("sprite_normal_map", 1);
            gbuffer_shader->setBool("has_normal_map", sprite_object->has_normal_map);
            gbuffer_shader->setFloat("z", sprite_object->z_layer);
            



            glBindVertexArray(quad_vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

    void drawLightBuffer(CameraData* camera_data) {
        glBindFramebuffer(GL_FRAMEBUFFER, camera_data->light_fbo);
        int pixel_width = static_cast<int>(width() * camera_data->screen_viewport_size.x());
        int pixel_height = static_cast<int>(height() * camera_data->screen_viewport_size.y());
        glViewport(0, 0, pixel_width, pixel_height);

        glClearColor(0.05f, 0.05f, 0.07f, 1.0f); // TODO: make customiced ambient light intenisty
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        light_buffer_shader->use();

        // fragment shader uniforms

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, camera_data->normal_map_texture);
        light_buffer_shader->setInt("normal_map", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, camera_data->depth_texture);
        light_buffer_shader->setInt("depth_map", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, camera_data->ignore_light_texture);
        light_buffer_shader->setInt("ignore_light_tex", 2);


        light_buffer_shader->setVec2(
            "camera_world_position",
            camera_data->camera->position
        );
        light_buffer_shader->setVec2(
            "camera_world_size",
            camera_data->camera->size
        );

        for(auto& point_light : camera_data->camera->getScene()->point_lights) {
            light_buffer_shader->setVec3(
                "light_position",
                point_light->position.x(),
                point_light->position.y(),
                point_light->z
            );
            light_buffer_shader->setVec3(
                "light_color",
                vec3<double>(
                    static_cast<double>(point_light->color.r) / 255,
                    static_cast<double>(point_light->color.g) / 255,
                    static_cast<double>(point_light->color.b) / 255
                )
            );
            light_buffer_shader->setFloat(
                "light_intenisty",
                point_light->intenisty
            );
            light_buffer_shader->setFloat(
                "light_linear_dropoff",
                point_light->linear_dropoff
            );
            light_buffer_shader->setFloat(
                "light_quadratic_dropoff",
                point_light->quadratic_dropoff
            );

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        }
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void finalLightPass(CameraData* camera_data) {
        glBindFramebuffer(GL_FRAMEBUFFER, camera_data->framebuffer_fbo);
        int pixel_width = static_cast<int>(width() * camera_data->screen_viewport_size.x());
        int pixel_height = static_cast<int>(height() * camera_data->screen_viewport_size.y());
        glViewport(0, 0, pixel_width, pixel_height);
        final_light_pass_shader->use();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, camera_data->color_texture);
        final_light_pass_shader->setInt("color_texture", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, camera_data->light_texture);
        final_light_pass_shader->setInt("light_texture", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, camera_data->ignore_light_texture);
        final_light_pass_shader->setInt("ignore_light_texture", 2);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
    }
    /*
        Draws the scenes bound to cameras on the screen
    */
    void drawSprites(int mode) {
        for( const auto &camera_data : cameras ) {
            if (camera_data->camera->getScene() == nullptr) {
                std::cerr << "Please bind a scene to the camera before drawing" << std::endl;
                exit(1);
            }
            drawGBuffer(camera_data);
            drawLightBuffer(camera_data);
            finalLightPass(camera_data);
            
            sprite_draw_shader->use();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            int screen_x = static_cast<int>(camera_data->screen_viewport_position.x() * width());
            int screen_y = static_cast<int>(camera_data->screen_viewport_position.y() * height());
            int screen_w = static_cast<int>(camera_data->screen_viewport_size.x() * width());
            int screen_h = static_cast<int>(camera_data->screen_viewport_size.y() * height());
            glViewport(screen_x, screen_y, screen_w, screen_h);

            sprite_draw_shader->setVec2("camera_world_position", camera_data->camera->position);
            sprite_draw_shader->setVec2("camera_world_size", camera_data->camera->size);
            
            sprite_draw_shader->setVec2("sprite_position", camera_data->camera->position);
            sprite_draw_shader->setVec2("sprite_offset", vec2<double>(0.0, 0.0));
            sprite_draw_shader->setVec2("sprite_size", camera_data->camera->size);

            glActiveTexture(GL_TEXTURE0);
            switch(mode){
                case 0:
                    glBindTexture(GL_TEXTURE_2D, camera_data->framebuffer_texture); 
                    break;
                case 1:
                    glBindTexture(GL_TEXTURE_2D, camera_data->color_texture); 
                    break;
                case 2:
                    glBindTexture(GL_TEXTURE_2D, camera_data->depth_texture); 
                    break;
                case 3:
                    glBindTexture(GL_TEXTURE_2D, camera_data->ignore_light_texture); 
                    break;
                case 4:
                    glBindTexture(GL_TEXTURE_2D, camera_data->light_texture); 
                    break;
                case 5:
                    glBindTexture(GL_TEXTURE_2D, camera_data->normal_map_texture); 
                    break;
            }
            sprite_draw_shader->setInt("sprite_texture", 0);

            glBindVertexArray(quad_vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

    void drawGUI() {
        gui_draw_shader->use();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        for(auto gui_group : gui_groups) {
            int viewport_pixel_x = static_cast<int>(width() * gui_group->viewport_position.x());
            int viewport_pixel_y = static_cast<int>(height() * gui_group->viewport_position.y());
            int viewport_pixel_width = static_cast<int>(width() * gui_group->viewport_size.x());
            int viewport_pixel_height = static_cast<int>(height() * gui_group->viewport_size.y());
            glViewport(viewport_pixel_x, viewport_pixel_y, viewport_pixel_width, viewport_pixel_height);

            for(auto gui_texture_element : gui_group->gui_texture_elements) {
                float element_pixel_width = gui_texture_element->width * gui_group->viewport_size.x() * width();
                float element_pixel_height = element_pixel_width * gui_texture_element->aspect_ratio;

                /* vertex shader uniforms */
                gui_draw_shader->setVec2(
                    "pixel_sprite_position",
                    vec2(
                        gui_texture_element->position.x() * viewport_pixel_width,
                        gui_texture_element->position.y() * viewport_pixel_height
                    )
                );
                gui_draw_shader->setVec2(
                    "pixel_sprite_size",
                    vec2<double>(element_pixel_width, element_pixel_height)
                );
                gui_draw_shader->setVec2(
                    "pixel_viewport_size",
                    vec2<double>(viewport_pixel_width, viewport_pixel_height)
                );
                /* fragment shader uniforms */
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, gui_texture_element->texture);
                gui_draw_shader->setInt("sprite_texture", 0);

                glBindVertexArray(quad_vao);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
        }
    }

    void draw(int mode) {

        // switch(has_anti_aliasing) {
        //     case 0: SDL_SetTextureScaleMode(framebuffer, SDL_SCALEMODE_NEAREST); break;
        //     case 1: SDL_SetTextureScaleMode(framebuffer, SDL_SCALEMODE_LINEAR); break;
        // }

        drawSprites(mode);
        drawGUI();

    }
};


};


#endif
