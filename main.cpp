#include "Frender/Frender.hh"
#include "Frender/FrenderTools.hh"
#include "Frender/Keys.hh"
#include "GLTFLoader.hh"
#include "glm/gtc/quaternion.hpp"
#include <ios>
#include <iostream>
// #define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <string>

#include "FrenderBoss.hh"

Frender::Window* window;
Frender::Renderer* renderer;
FrB::Player player;
FrB::BallPhysics physics;
glm::mat4 camera;

FrenderTools::RenderGroup rg;

void loop(float delta)
{
    player.update(delta);
    physics.update(delta);

    if (window->isKeyJustPressed(FRENDER_KEY_ESCAPE))
    {
        std::cout << "Releasing\n";
        window->setMouseMode(Frender::Regular);
    }

    if (window->isKeyJustPressed(FRENDER_KEY_ENTER))
    {
        std::cout << "Capturing\n";
        window->setMouseMode(Frender::Captured);
    }
}

int main(int, char**)
{
    Frender::Window w({1280, 720, "Hello Frender"});
    w.setVsync(false);
    // w.setVsync(true);

    Frender::Renderer r(1280, 720);
    renderer = &r;
    window = &w;

    // Camera
    glm::mat4 cam_mat;
    cam_mat = glm::translate(cam_mat, glm::vec3(0, 0, 1));
    r.setCamera(cam_mat);
    r.fov_rad = glm::radians(120.0f);
    camera = cam_mat;
    r.far_distance = 250;

    // Texture
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf("Assets/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr", &width, &height, &channels,
                             STBI_rgb_alpha);
    std::cout << channels << "\n";

    if (data)
    {
        // auto tex = r.createTexture(width, height, data);
        // r.getMaterial(mat)->textures.set("tex", tex);
        r.setSkybox(width, height, data);
    }
    else
    {
        std::cerr << "Waaaaaaaaaaaaaa image not found\n";
    }

    stbi_image_free(data);

    // light = r.createPointLight(glm::vec3(0.120, -0.870, 2.030), glm::vec3(104, 0, 0), 4);
    // auto light2 = r.createPointLight(glm::vec3(0.120, -0.870, -2.030), glm::vec3(0, 4, 0), 4);
    // auto light3 = r.createPointLight(glm::vec3(0, 2, 0), glm::vec3(0, 0, 4), 4);
    // auto dlight = r.createDirectionalLight(glm::vec3(1.5, 1.5, 1.5), glm::vec3(0.2f, -1.0f, 0.3f));
    // auto dlight0 = r.createDirectionalLight(glm::vec3(1, 1, 1), glm::vec3(-0.2f, -1.0f, -0.3f));
    // auto dlight1 = r.createDirectionalLight(glm::vec3(1, 1, 1), glm::vec3(0.2f, -1.0f, -0.3f));
    // auto dlight2 = r.createDirectionalLight(glm::vec3(1, 1, 1), glm::vec3(-0.2f, -1.0f, 0.3f));
    // auto dlight3 = r.createDirectionalLight(glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 1.0f, 0));

    // camera_position = glm::translate(glm::mat4(), glm::vec3(0, 5, 0));

    // w.setMouseMode(Frender::Captured);

    // Load Level
    auto objs = loadModel(renderer, "Assets/TestLevel.glb");
    for (auto o : objs)
    {
        renderer->getMaterial(renderer->getRenderObjectTraits(o).material)->uniforms.set("roughness", 0.4f);
    }

    physics = FrB::BallPhysics(renderer);
    player = FrB::Player(renderer, window, &physics);

    auto objs2 = loadModel(renderer, "Assets/Asteroid.glb");
    srand(time(NULL));
    for (int i = 0; i < 250; i++)
    {
        rg = FrenderTools::RenderGroup();
        float rr = 0.5 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0 - 0.5));
        for (auto o : objs2)
        {
            auto rots = renderer->getRenderObjectTraits(o);
            auto new_ro =
                renderer->createRenderObject(rots.mesh, rots.material, glm::scale(rots.transform, glm::vec3(rr)));
            rg.addRenderObject(new_ro);
        }

        float r1 = -1.0 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (1.0 - -1.0)));
        float r2 = -1.0 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (1.0 - -1.0)));
        float r3 = -1.0 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (1.0 - -1.0)));
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        physics.addBall(glm::vec3(rand() % 150 - 75, rand() % 150 - 75, rand() % 150 - 75), glm::vec3(r1, r2, r3), r,
                        rr, rr, rg);
    }

    // physics.addBall(glm::vec3(5, 0, -5), glm::vec3(-0.2, 0, 0), 1.5, 1, 1, rg2);

    w.mainloop(&r, loop);
}