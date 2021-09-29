#include "Frender/Frender.hh"
#include "Frender/FrenderTools.hh"
#include "Frender/Keys.hh"
#include "FrenderBoss.hh"
#include "GLTFLoader.hh"
#include "glm/gtc/matrix_transform.hpp"

FrB::Player::Player(Frender::Renderer* renderer, Frender::Window* window, FrB::BallPhysics* physics)
    : renderer(renderer), window(window), physics(physics)
{
    auto objs2 = loadModel(renderer, "Assets/Cocpit.glb");
    for (auto i : objs2)
    {
        renderer->getMaterial(renderer->getRenderObjectTraits(i).material)->uniforms.set("roughness", 0.8f);
        renderer->getMaterial(renderer->getRenderObjectTraits(i).material)->uniforms.set("metalness", 1.0f);
        rg.addRenderObject(i);
    }

    cp_offset = glm::translate(glm::mat4(), glm::vec3(0, -1, -0.2));
    proj_offset = glm::translate(glm::mat4(), glm::vec3(0, -0.8, -2.3));

    auto brg = FrenderTools::RenderGroup();
    ball = physics->addBall(transform * cp_offset * glm::vec4(0, 0, 0, 1), glm::vec3(0, 0, 0), 0, 2, 25, brg, 1);

    projectiles = ProjectileManager(renderer, window, physics);
}

void FrB::Player::update(float delta)
{
    if (window->isKeyDown(FRENDER_KEY_W))
    {
        velocity.z -= 5 * delta;
    }

    if (window->isKeyDown(FRENDER_KEY_S))
    {
        velocity.z += 5 * delta;
    }

    if (window->isKeyJustPressed(FRENDER_KEY_SPACE))
    {
        projectiles.spawnProjectile(transform * proj_offset, velocity);
    }

    if (physics->didBallCollide(ball))
    {
        velocity *= 0.75 * delta;
    }

    transform = glm::translate(transform, velocity * delta);

    auto mouse_offset = window->getMouseOffset();
    transform = glm::rotate(transform, mouse_offset.x * 0.0005f, glm::vec3(0, 0, 1));
    transform = glm::rotate(transform, mouse_offset.y * 0.0005f, glm::vec3(1, 0, 0));

    renderer->setCamera(transform);
    rg.setTransform(transform * cp_offset);
    rg.applyTransform();

    physics->setBallPosition(ball, transform * cp_offset * glm::vec4(0, 0, 0, 1));
    physics->setBallVelocity(ball, velocity);

    projectiles.update(delta);
}