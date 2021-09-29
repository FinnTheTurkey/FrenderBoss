#include "Frender/Frender.hh"
#include "Frender/FrenderTools.hh"
#include "Frender/Keys.hh"
#include "FrenderBoss.hh"
#include "GLTFLoader.hh"
#include "glm/gtc/matrix_transform.hpp"

static const std::vector<glm::vec3> colors = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

FrB::ProjectileManager::ProjectileManager(Frender::Renderer* renderer, Frender::Window* window,
                                          FrB::BallPhysics* physics)
    : renderer(renderer), window(window), physics(physics)
{

    for (auto c : colors)
    {
        auto mat = renderer->createMaterial();
        renderer->getMaterial(mat)->uniforms.set("color", c);
        materials.push_back(mat);
    }

    ros = loadModel(renderer, "Assets/Projectile.glb");
}

void FrB::ProjectileManager::spawnProjectile(glm::mat4 transform, glm::vec3 velocity)
{
    FrenderTools::RenderGroup rg;
    for (auto i : ros)
    {
        auto rots = renderer->getRenderObjectTraits(i);
        auto new_ro = renderer->createRenderObject(rots.mesh, materials[mat_on], rots.transform);
        rg.addRenderObject(new_ro);
    }
    rg.setTransform(transform);
    rg.applyTransform();

    // auto light = renderer->createPointLight(transform[3], colors[mat_on] * 2.0f, 4);
    int ball =
        physics->addBall(transform[3], glm::mat3(transform) * (velocity + glm::vec3(0, 0, -10)), 0, 0.3, 1, rg, 1);

    projectiles.push_back({rg, transform, ball});

    mat_on++;
    if (mat_on >= materials.size())
    {
        mat_on = 0;
    }
}

void FrB::ProjectileManager::update(float delta)
{
    for (int i = 0; i < projectiles.size(); i++)
    {
        //     // projectiles[i].transform = glm::translate(projectiles[i].transform, glm::vec3(0, 0, -30 * delta));

        //     // projectiles[i].rg.setTransform(projectiles[i].transform);
        //     // projectiles[i].rg.applyTransform();
        auto pos = physics->getBallPosition(projectiles[i].ball);
        if (glm::abs(glm::distance(glm::vec3(projectiles[i].transform[3]), pos)) > 200)
        {
            physics->removeBall(projectiles[i].ball);
            // TODO: Make this line not sometimes cause segfaults down the road
            projectiles[i].rg.destroy();
            projectiles.erase(projectiles.begin() + i);
            std::cout << "Removing ball!\n";
            i--;
        }
        //     renderer->setLightPosition(projectiles[i].light, pos);
        //     auto new_pos = renderer->getLightPosition(projectiles[i].light);

        //     if (pos != new_pos)
        //     {
        //         std::cout << "Not lit :(\n";
        //     }
    }
}