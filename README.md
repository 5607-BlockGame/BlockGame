# BlockGame
A Minecraft-like POC game made in C++

# Files

```text
src/utils           OpenGL helper functions
src/Scene           combines helper functions and map data to display scene
src/generator/*     world generator
src/repr/*          representations of data (Chunk, World, Block), etc.
src/main            really messy right now should probably be separated into multiple files
```

# Installation

## MacOS

- Make sure to clone the repo recursively `git clone {url} --recursive`
  - If you have already cloned do `git submodule update --init`
- Install all dependencies listed in `dependencies.sh`. You can install with `./dependencies.sh`

# Running

- You must run from the project root directory. This can be done in CLion as seen below:

![Content Root](.github/content-root.png)


# Final Write-Up

## Team
- Andrew Gazelka
- Quynh Do
- Allison Miller

## Project Description & Overview

Our final project is a real-time minimalistic block game inspired by Minecraft. We use OpenGL and C++ (20).
All the goals we aimed to accomplish are listed below, and the ones that we were able to achieve are marked with an x as follows:

- [ ] Terrain generation
  - [x] Minimal flat terrain generation
  - [x] Hills
  - [ ] Trees and other structures
  - [ ] Caves (if we get time)
- [x] Saving worlds
- [x] Loading worlds
- [x] Displaying blocks
- [x] Hand animation
- [x] Simple Physics (jumping)
- [x] Flying (activate and de-activate)
- [x] Collision
- [x] Cross hair
- [x] Block selection indicator
- [x] Building
- [x] Break blocks
- [ ] Different block types

### Main Obstacles

The main technical obstacle was the large number of blocks generated for the game; Andrew had to optimize the rendering so that the game could run smoothly, which necessitated removing the textures for the blocks and using a placeholder color instead. Another issue with the large quantity of blocks was aliasing between the models when viewed from a distance; setting the near clipping plane to be close to the camera allowed us to include a faithful representation of the Minecraft hand while mining but exacerbated the aliasing issue, so the hand and the near clipping plane both had to be moved out from the camera a ways (which works, but doesn't quite look as we imagined it looking at first). Quynh and Allison were also not at all familiar with the code base that Andrew set up, so it took them quite a while to understand it well enough to implement the breaking and building blocks features along with the selection indicator. Converting between 2D and 3D perspective was another obstacle that we encountered since we mostly learned about 3D graphics in our class. Allison also had issues initially setting up CMake to work with the project and had to switch to a new IDE partway through the project to avoid having to set up a new project every time new code was committed.

## Connection to our class

Our project is an extension of what we learned in Project 4, which involves real-time 3D game programming in OpenGL. Our game demonstrates the rendering pipeline. We have simple animation of an object hitting the blocks. Moreover, we use both perspective and orthographic projection in the game. The perspective projection is for rendering the 3D scene, and the orthographic projection is for rendering the cross-hair on the 2D screen. We implement a dynamic environment where the player can interact with the blocks by breaking or building a new one. User interface includes mouse and keyboard control, jumping, block selection indicator, and simple Imgui integration. Lighting and Shading are done using the provided shader code from project 4.

## Key Features and Images

### Terrain Generation
Terrain is generated using [Open Simplex Noise](https://github.com/SRombauts/SimplexNoise). Each chunk (16x16x256) area
is generated independently of other chunks. If a chunk has not been saved to disk, it is generated. If it has been 
generated and is in memory, it is loaded from memory. If it is on disk, it is loaded from disk.

### Optimizations
Only the top-most blocks of the world are rendered. This could be improved to only render polygons that can be shown
(we are still rendering the backs of blocks), but we have not reached that point yet.

### Movement
Player can control camera rotation using the mouse and camera movement using the keyboard.

Keys map:

- A/D - move left/right
- W/S - move forward/backward
- G - enable/disable gravity (toggle flying mode)
- Space - Jump (not flying mode) / Move up (flying mode)
- Left shift - Move down (flying mode)

#### Jumping
![Jumping](https://media.giphy.com/media/ynfcaq0GS4gNgkB2zo/giphy.gif)

#### Flying
![Flying](https://media.giphy.com/media/f4s6bkmBlsJXvfGz2e/giphy.gif)


### Breaking blocks
- Press and hold the mouse to break the blocks. The player can only break blocks near them within a set radius. A green selection indicator will appear when you begin breaking a block if that block is within range.

![Breaking blocks](https://media.giphy.com/media/phcRtsXEi5xnQYLlFT/giphy.gif)

### Building blocks
- Move the cross hair to the targeted position and press P to place a block.

![Building](https://media.giphy.com/media/CX0RURzsdP9TAzwqyV/giphy.gif)

## Future Work

Since this is a 3D game, there is much room to improve in the future. We could complete the terrain generation process to include trees, caves, and other structures. A priority would be to optimize our terrain rendering enough so that we could add block textures for better visualization. Since the game is based on ideas from Minecraft, incorporating textures would allow us to show the progress of a block being broken by using textures that look more and more broken. Including particle effects during the block breaking process would help with the realism of the animation, though this idea would be significantly more advanced than some of the others. We could also introduce a wider range of block types to display different materials with different interactions and effects on the environment, like water or lava in addition to the standard solid types such as dirt or stone.  User interaction could also be improved to be smoother and more accurate - at the moment, the gravity when the flight mode is turned off is not physically realistic, as the user simply drops immediately and at a constant rate, which makes navigating while jumping a bit clunky.
