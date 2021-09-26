#include "Frender/Frender.hh"
#include "Frender/FrenderTools.hh"
#include "Frender/Keys.hh"
#include "FrenderBoss.hh"
#include "GLTFLoader.hh"
#include "glm/gtc/matrix_transform.hpp"

static const std::vector<glm::vec3> colors = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

FrB::ProjectileManager::ProjectileManager(Frender::Renderer* renderer, Frender::Window* window)
    : renderer(renderer), window(window)
{

    for (auto c : colors)
    {
        auto mat = renderer->createMaterial();
        renderer->getMaterial(mat)->uniforms.set("color", c);
        materials.push_back(mat);
    }

    ros = loadModel(renderer, "Assets/Projectile.glb");
}

void FrB::ProjectileManager::spawnProjectile(glm::mat4 transform)
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

    auto light = renderer->createPointLight(transform[4], colors[mat_on], 4);

    projectiles.push_back({rg, light, transform});

    mat_on++;
    if (mat_on >= materials.size())
    {
        mat_on = 0;
    }
}

void FrB::ProjectileManager::update(float delta)
{
}