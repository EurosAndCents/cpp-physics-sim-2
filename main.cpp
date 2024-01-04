#include "./objects/ball.h"

#include "constants.h"
#include INCLUDE_RAYLIB
#include INCLUDE_RAYMATH

#include <iostream>
#include <vector>
#include <memory>

// Iterates through the ballbound_collisions_to_occur vector and resolves each collision about to happen
void resolveCollisions(std::vector<std::pair<std::shared_ptr<Ball>, Vector2>>& ballbound_collisions_to_occur) 
{
    for(auto& collision_event /*std::pair<std::shared_ptr<Ball>, Vector2 normal>*/ : ballbound_collisions_to_occur) {
        std::shared_ptr<Ball> ball = std::get<0>(collision_event);
        Vector2 normal = std::get<1>(collision_event);
        unsigned int i{};

        if(Vector2Equals(normal, Vector2Zero())) continue; // If no collision normal, skip collision loop

        Vector2 v = Vector2Reflect(ball->velocity,normal); // -e=v/u turns to e=v/u (as it is reflected)
        v = Vector2Scale(v, RESTITUTION); // e=v/u -> v=e*u

        ball->velocity = v;
    }
}

// Check ball (a) against each bound on the screen and return a std::pair(unique_ptr<Ball>&, Vector2 normal)
std::pair<std::shared_ptr<Ball>,Vector2> ballBoundCollision(std::vector<std::shared_ptr<Ball>>& ball_instances, std::shared_ptr<Ball>& a) 
{
    Vector2 collision_normal{};

    // If outside of left bound
    if((a->position.x <= a->radius)) {
        collision_normal.x = 1;
    }

    // If outside of right bound
    if((a->position.x >= WIDTH-a->radius)) {
        collision_normal.x = -1;
    }

    // If outside of top bound
    if((a->position.y <= a->radius)) {
        collision_normal.y = 1;
    }
    
    // If outside of bottom bound
    if((a->position.y >= HEIGHT-a->radius)) {
        collision_normal.y = -1;
    }

    // !-- We're using std::move(a) so that std::pair becomes the new owner of std::unique_ptr<Ball>, 
    //     instead of creating a copy of it (which is not allowed as then 2 ptrs would have ownership of the Ball instance)
    return std::make_pair(a, collision_normal);
}

int main()
{
    InitWindow(WIDTH,HEIGHT,"physics-sim-2");
    SetTargetFPS(FRAMERATE);

    // !-- We're using std::shared_ptr so that std::vector has ownership of the Ball instances, meaning they are automatically
    //     deleted as the vector goes out of scope, but so that functions can duplicate the pointers.
    std::vector<std::shared_ptr<Ball>> ball_instances{};
    ball_instances.push_back(std::make_shared<Ball>(CLITERAL(Vector2){50.0f,50.0f}, 20.0f));

    std::vector<std::pair<std::shared_ptr<Ball>,Vector2>> ballbound_collisions_to_occur{};
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Add Force Downwards
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            ball_instances[0]->addForce(CLITERAL(Vector2){0.0f,1.0f});
        } else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            ball_instances[0]->addForce(CLITERAL(Vector2){0.0f,-1.0f});
        }
        
        // Add Force Upwards
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            ball_instances[0]->addForce(CLITERAL(Vector2){0.0f,-1.0f});
        } else if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
            ball_instances[0]->addForce(CLITERAL(Vector2){0.0f,1.0f});
        }

        ballbound_collisions_to_occur.clear();
        for(auto& ball : ball_instances) {
            // Handle Ball<->Bound collisions
            ballbound_collisions_to_occur.push_back(ballBoundCollision(ball_instances, ball));

            // Handle Ball<->Ball collisions

            // Resolve collisions
            resolveCollisions(ballbound_collisions_to_occur);

            // Update each Ball at the end of the frame
            ball->resolve(GetFrameTime());
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}