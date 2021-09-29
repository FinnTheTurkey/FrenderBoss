#ifndef FRENDERBOSS_HH
#define FRENDERBOSS_HH

#include "Frender/Frender.hh"
#include "Frender/FrenderTools.hh"
#include <queue>
#include <vector>

namespace FrB
{

struct Ball
{
    // Position and velocity are in the global frame of reference
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 new_velocity;

    float acceleration;
    float radius;
    float mass;
    int collider;
    int layer;
    int col_id;
    FrenderTools::RenderGroup rg;

    glm::vec3* minima;
    glm::vec3* maxima;
};

struct Extrema
{
    enum ExtremaType
    {
        Minima,
        Maxima
    };

    ExtremaType et;

    glm::vec3 position;
    int ball_id;
    glm::vec3* index;
};

class BallPhysics
{
  public:
    BallPhysics(){};
    BallPhysics(Frender::Renderer* renderer);

    int addBall(glm::vec3 position, glm::vec3 velocity, float acceleration, float radius, float mass,
                FrenderTools::RenderGroup rg, int layer = 0, int col_id = 0);

    void removeBall(int ball);

    void setBallVelocity(int ball, glm::vec3 velocity);
    glm::vec3 getBallVelocity(int ball);

    void setBallPosition(int ball, glm::vec3 position);
    glm::vec3 getBallPosition(int ball);

    int didBallCollide(int ball);

    /*
    Moves all the balls, and deals with collisions
    */
    void update(float delta);

  private:
    glm::vec3* addExtrema(Extrema e);
    void removeExtrema(glm::vec3* ex_index);

    Frender::Renderer* renderer;

    std::vector<Ball> ballz;
    std::queue<int> reuse_queue;

    std::array<std::vector<Extrema>, 3> broad_phase;
};

struct Projectile
{
    FrenderTools::RenderGroup rg;
    // Frender::LightRef light;
    glm::mat4 transform;
    int ball;
};

class ProjectileManager
{
  public:
    ProjectileManager(){};
    ProjectileManager(Frender::Renderer* renderer, Frender::Window* window, FrB::BallPhysics* physics);

    void update(float delta);
    void spawnProjectile(glm::mat4 transform, glm::vec3 velocity);

  private:
    Frender::Renderer* renderer;
    Frender::Window* window;
    FrB::BallPhysics* physics;

    std::vector<Frender::RenderObjectRef> ros;
    std::vector<uint32_t> materials;
    std::vector<Projectile> projectiles;
    int mat_on = 0;
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

    ProjectileManager projectiles;

    glm::mat4 transform;
    glm::mat4 cp_offset;
    glm::mat4 proj_offset;
    glm::vec3 velocity;
    int ball;
};
} // namespace FrB

#endif