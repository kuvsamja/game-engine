## SETUP:

### Features
- [x] Add gui elements
- [x] Make text gui
- [x] Add sprite drawing
- [x] input handling
  - [ ] getButtonDown() and getButtonUp()
- [ ] general event handling
- [x] make screen handle camera creation
- [x] sprite z ordering
- [x] make mutable sprite z_order
- [x] make mutable gui z_order
- [x] sprite deletion from the scene
  - [ ] audio stuff
- [ ] make lighting
- [ ] try making a way to prevent mixels when making pixelart
- [ ] add proper window resizing handling
- [ ] expose a custom shader maker api
- [x] transparent sprites
- [x] transparent gui elements
- [x] make sprites autodetect aspect ratio
- [x] make camera size and viewport aspect ratio sync
  - [ ] fix window scaling
- [ ] unicode font support
- [ ] autocalculate font size for the text box size


### Stucture and code
  - [ ] unbind textures after binding or use
- [x] better error messages
  - [ ] make stuff non copyable and constant
- [x] make shader.hpp use c++ file streams
- [x] mabye migrate all math to glm OR migrate the shader uniforms to the custom vec2/vec3
- [x] switch to opengl ):
  - [ ] fix various aspect ratio related issues
- [ ] better text resizing
- [ ] move to the other std_truetype api
  - [ ] Handle who owns the Font object so it doesnt go out of scope before stuff using it and doesnt leak memory
  - [ ] standardize member names
  - [ ] virtual draw()

## FURTHER FEATURES
- [ ] Add configurable custom drawing logic
- [ ] Make some objects
- [ ] add multiple sprites per object
- [ ] add animation support
- [ ] hitbox implementation
- [ ] make scenes load from files
- [ ] rotations
- [ ] make multiple sprites use the same texture for performance
- [ ] move to the gpu

## Physics
- [ ] Make some collisions
- [ ] Rigidbody simulation
- [ ] Softbodies
 
## goal
- [ ] make a gui editor
