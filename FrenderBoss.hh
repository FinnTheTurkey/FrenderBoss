#ifndef FRENDERBOSS_HH
#define FRENDERBOSS_HH

#include "Frender/Frender.hh"
#include "Frender/FrenderTools.hh"
#include <queue>
#include <vector>

namespace FrB
{

struct Projectile
{
    FrenderTools::RenderGroup rg;
    Frender::LightRef light;
    glm::mat4 transform;
};

class ProjectileManager
{
  public:
    ProjectileManager(){};
    ProjectileManager(Frender::Renderer* renderer, Frender::Window* window);

    void update(float delta);
    void spawnProjectile(glm::mat4 transform);

  private:
    Frender::Renderer* renderer;
    Frender::Window* window;

    std::vector<Frender::RenderObjectRef> ros;
    std::vector<uint32_t> materials;
    std::vector<Projectile> projectiles;
    int mat_on = 0;
};

struct Ball
{
    // Position and velocity are in the global frame of reference
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 new_velocity;

    float acceleration;
    float radius;
    float mass;
    bool did_collide;
    FrenderTools::RenderGroup rg;
};

class BallPhysics
{
  public:
    BallPhysics(){};
    BallPhysics(Frender::Renderer* renderer);

    int addBall(glm::vec3 position, glm::vec3 velocity, float acceleration, float radius, float mass,
                FrenderTools::RenderGroup rg);

    void setBallVelocity(int ball, glm::vec3 velocity);
    glm::vec3 getBallVelocity(int ball);

    void setBallPosition(int ball, glm::vec3 position);
    glm::vec3 getBallPosition(int ball);

    bool didBallCollide(int ball);

    /*
    Moves all the balls, and deals with collisions
    */
    void update(float delta);

  private:
    Frender::Renderer* renderer;

    std::vector<Ball> ballz;
    std::queue<int> reuse_queue;
};

class Player
{
  public:
    Player(){};
    Player(Frender::Renderer* renderer, Frender::Window* window, FrB::BallPhysics* physics);
    void reset();
    void update(float delta);

  private:
    FrenderTools::RenderGroup rg;
    Frender::Renderer* renderer;
    Frender::Window* window;
    FrB::BallPhysics* physics;

    glm::mat4 transform;
    glm::mat4 cp_offset;
    glm::vec3 velocity;
    int ball;
};
} // namespace FrB

#endif