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
                              FrenderTools::RenderGroup rg, int layer, int col_id)
{

    int fr = 0;
    if (reuse_queue.empty())
    {
        ballz.push_back({position, velocity, velocity, acceleration, radius, mass, -1, col_id, layer, rg});
        fr = ballz.size() - 1;
    }
    else
    {
        ballz[reuse_queue.front()] = {position, velocity, velocity, acceleration, radius, mass, -1, col_id, layer, rg};
        fr = reuse_queue.front();
        reuse_queue.pop();
    }

    // Add to broad phase
    // glm::vec3 positiona = position - glm::vec3(radius);
    // glm::vec3 positionb = position + glm::vec3(radius);
    // ballz[fr].minima = addExtrema({Extrema::Minima, positiona, fr});
    // ballz[fr].maxima = addExtrema({Extrema::Maxima, positionb, fr});

    return fr;
}

void FrB::BallPhysics::removeBall(int ball)
{
    ballz[ball].radius = -1;
    reuse_queue.emplace(ball);
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

int FrB::BallPhysics::didBallCollide(int ball)
{
    return ballz[ball].collider;
}

glm::vec3* FrB::BallPhysics::addExtrema(Extrema e)
{
    glm::vec3 output;

    if (e.index == nullptr)
    {
        e.index = new glm::vec3();
    }

    for (int i = 0; i < 3; i++)
    {
        // Add extrema in one axis
        auto minima_it =
            std::upper_bound(broad_phase[i].begin(), broad_phase[i].end(), e,
                             [i](const Extrema& value, Extrema& info) { return value.position[i] < info.position[i]; });

        auto mit = broad_phase[i].insert(minima_it, e);
        output[i] = mit - broad_phase[i].begin();
        (*e.index)[i] = output[i];

        // Update the rest of the indicies
        for (int j = output[i] + 1; j < broad_phase[i].size(); j++)
        {
            (*broad_phase[i][j].index)[i] = j;
        }
    }

    return e.index;
}

void FrB::BallPhysics::removeExtrema(glm::vec3* ex_index)
{
    // Remove it
    for (int i = 0; i < 3; i++)
    {
        broad_phase[i].erase(broad_phase[i].begin() + (*ex_index)[i]);

        // Update the rest of the indices
        for (int j = (*ex_index)[i]; j < broad_phase[i].size(); j++)
        {
            (*broad_phase[i][j].index)[i] = j;
        }
    }

    // Free the memory
    delete ex_index;
}

void FrB::BallPhysics::update(float delta)
{
    // Step 1: Update all the positions from last frame
    // for (int i = 0; i < broad_phase.size(); i++)
    // {
    //     glm::vec3 positiona =
    //         ballz[broad_phase[0][i].ball_id].position - glm::vec3(ballz[broad_phase[0][i].ball_id].radius);
    //     glm::vec3 positionb =
    //         ballz[broad_phase[0][i].ball_id].position + glm::vec3(ballz[broad_phase[0][i].ball_id].radius);

    //     if (broad_phase[0][i].et == Extrema::Minima)
    //     {
    //         broad_phase[0][i].position = positiona;
    //     }
    //     else
    //     {
    //         broad_phase[0][i].position = positionb;
    //     }
    // }

    // // Step 2: Sort the broad phase
    // // Basically just your standard insertion sort
    // for (int d = 0; d < 3; d++)
    // {
    //     int j;
    //     for (int i = 1; i < broad_phase[d].size(); i++)
    //     {
    //         Extrema key = broad_phase[d][i];

    //         j = i;
    //         while (j > 0 && broad_phase[d][j - 1].position[d] > key.position[d])
    //         {
    //             broad_phase[d][j] = broad_phase[d][j - 1];

    //             // Update index
    //             (*broad_phase[d][j].index)[d] = j;

    //             j--;
    //         }

    //         broad_phase[d][j] = key;
    //         (*broad_phase[d][j].index)[d] = j;
    //     }
    // }

    // // Step 3: Check and deal with collisions
    // for (int i = 0; i < ballz.size(); i++)
    // {
    //     // Ball are small enough that if there's _anything_ overlapping we'll check for collisions
    //     if ((*ballz[i].minima)[0] + 1 != (*ballz[i].maxima)[0] && (*ballz[i].minima)[1] + 1 != (*ballz[i].maxima)[1]
    //     &&
    //         (*ballz[i].minima)[2] + 1 != (*ballz[i].maxima)[2])
    //     {
    //         // We actually have to check
    //         // We only have to check one axis,
    //         // because we can brute force check the rest
    //         for (int p = (*ballz[i].minima)[0]; p < (*ballz[i].maxima)[0]; p++)
    //         {
    //             // Get that extrema's ball
    //             int ball = broad_phase[0][p].ball_id;

    //             // Check the other axis
    //             // I once again forgot about p[i[i]p] situations
    //             // if ()
    //         }
    //     }
    // }

    // Collision detection and stuff
    for (int i = 0; i < ballz.size(); i++)
    {
        if (ballz[i].radius == -1)
            continue;

        ballz[i].collider = -1;
        auto position = ballz[i].position;
        auto radius = ballz[i].radius;

        // This is horribly inefficient, but that shouldn't matter
        for (int j = 0; j < ballz.size(); j++)
        {
            if (j == i || ballz[j].radius == -1)
                continue;

            if (ballz[j].layer == ballz[i].layer)
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
                ballz[i].collider = ballz[j].col_id;

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
        if (ballz[i].radius == -1)
            continue;

        ballz[i].velocity =
            ballz[i].new_velocity + (ballz[i].acceleration * delta * glm::normalize(ballz[i].new_velocity));
        ballz[i].position += ballz[i].velocity * delta;
        ballz[i].rg.setTransform(glm::translate(glm::mat4(), ballz[i].position));
        ballz[i].rg.applyTransform();
    }
}