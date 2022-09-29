# physics-engine

This is a Visual Studio 2022 project.

This was created to serve as a skill development lab for my senior year capstone class.

The following concepts were learned:
- Application structure and flow (init -> loop -> destroy, frame timers, physics/rendering frequencies).
- Procedural texture generation (XOR texture, perlin noise, file loading, solid colored).
- OpenGL buffer objects (vertex buffer objects, vertex array objects, shader programs).
- Linear algebra (matrix multiplication, vector math and scalar multiplication).
- Data structures (meshes, window, vertex storage).

This is part one of two for this lab. The other portion will consist of an implementation of ECS (entity-component-system) architecture and the GJK algorithm and collision culling methods.

Dependencies:
- glad (for OpenGL defines).
- GLFW (for low level window creation and management).
- glm (linear algebra).
- KHR (for GLFW implementation).
- Reputeless for their Perlin Noise C++ implementation (https://github.com/Reputeless/PerlinNoise).
- Sean Barrett for the stb_image.h library (https://github.com/nothings/stb).
