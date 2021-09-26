#include "Frender/Frender.hh"
#include "Frender/FrenderTools.hh"
#include "Frender/Keys.hh"
#include "FrenderBoss.hh"
#include "GLTFLoader.hh"
#include "glm/detail/func_geometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

FrB::BallPhysics::BallPhysics(Frender::Renderer* renderer) : renderer(renderer)
{
}

int FrB::BallPhysics::addBall(glm::vec3 position, glm::vec3 velocity, float acceleration, float radius, float mass,
                              FrenderTools::RenderGroup rg)
{
    if (reuse_queue.empty())
    {
        ballz.push_back({position, velocity, velocity, acceleration, radius, mass, false, rg});
        return ballz.size() - 1;
    }

    ballz[reuse_queue.front()] = {position, velocity, velocity, acceleration, radius, mass, false, rg};
    int fr = reuse_queue.front();
    reuse_queue.pop();

    return fr;
}

void FrB::BallPhysics::setBallVelocity(int ball, glm::vec3 velocity)
{
    ballz[ball].velocity = velocity;
}

glm::vec3 FrB::BallPhysics::getBallVelocity(int ball)
{
    return ballz[ball].velocity;
}

void FrB::BallPhysics::setBallPosition(int ball, glm::vec3 position)
{
    ballz[ball].position = position;
}

glm::vec3 FrB::BallPhysics::getBallPosition(int ball)
{
    return ballz[ball].position;
}

bool FrB::BallPhysics::didBallCollide(int ball)
{
    return ballz[ball].did_collide;
}

void FrB::BallPhysics::update(float delta)
{
    // Collision detection and stuff
    for (int i = 0; i < ballz.size(); i++)
    {
        ballz[i].did_collide = false;
        auto position = ballz[i].position;
        auto radius = ballz[i].radius;

        // This is horribly inefficient, but that shouldn't matter
        for (int j = 0; j < ballz.size(); j++)
        {
            if (j == i)
                continue;

            // Pythagorian theorum once again saves the day
            // Use squared radius to save doing a sqrt
            float distance2 = abs(pow(ballz[j].position.x - position.x, 2) + pow(ballz[j].position.y - position.y, 2) +
                                  pow(ballz[j].position.z - position.z, 2));

            if (distance2 < pow(radius + 0.2, 2) + pow(ballz[j].radius + 0.2, 2))
            {
                // We are colliding
                // Since this is a bad physics simulation, we can change the velocities right here
                // We are only changing i's velocity since j will do it's own collision detection

                // Get collision normal:
                auto normal = glm::normalize(ballz[i].position - ballz[j].position);
                ballz[i].new_velocity =
                    normal * (abs(ballz[i].velocity.x + ballz[i].velocity.y + ballz[i].velocity.z) / 3) * ballz[j].mass;
                // normal * (ballz[i].velocity / glm::normalize(ballz[i].velocity)) * ballz[j].mass; // * 1.25f;
                // ballz[i].acceleration += 1;
                ballz[i].did_collide = true;

                // if (ballz[i].new_velocity == glm::vec3(0, 0, 0))
                // {
                // For some reason this makes the ball dissapear
                // ballz[i].new_velocity = ballz[i].velocity * 0.5f;
                // }
            }
        }
    }

    for (int i = 0; i < ballz.size(); i++)
    {
        ballz[i].velocity =
            ballz[i].new_velocity + (ballz[i].acceleration * delta * glm::normalize(ballz[i].new_velocity));
        ballz[i].position += ballz[i].velocity * delta;
        ballz[i].rg.setTransform(glm::translate(glm::mat4(), ballz[i].position));
        ballz[i].rg.applyTransform();
    }
}