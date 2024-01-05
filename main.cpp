#include "./objects/ball.h"

#include "constants.h"
#include INCLUDE_RAYLIB
#include INCLUDE_RAYMATH

#include <iostream>
#include <vector>
#include <memory> // std::shared_ptr for tracking Ball instances

// Iterates through the ballbound_collisions_to_occur vector and resolves each collision about to happen
void resolveCollisions(std::vector<std::pair<std::shared_ptr<Ball>, Vector2>>& ballbound_collisions_to_occur) 
{
    for(auto& collision_event /*std::pair<std::shared_ptr<Ball>, Vector2 normal>*/ : ballbound_collisions_to_occur) {
        std::shared_ptr<Ball> ball = std::get<0>(collision_event);
        Vector2 normal = std::get<1>(collision_event);

        if(Vector2Equals(normal, Vector2Zero())) continue; // If no collision normal, skip collision loop

        Vector2 v = Vector2Reflect(ball->velocity,normal); // -e=v/u turns to e=v/u (as it is reflected)
        v = Vector2Scale(v, RESTITUTION); // e=v/u -> v=e*u

        ball->velocity = v;
    }
}

// Check ball (a) against each bound on the screen and return a std::pair(unique_ptr<Ball>&, Vector2 normal)
std::pair<std::shared_ptr<Ball>,Vector2> ballBoundCollision(std::vector<std::shared_ptr<Ball>>& ball_instances, 
    std::shared_ptr<Ball>& a) 
{
    Vector2 collision_normal{Vector2Zero()};

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

    return std::make_pair(a, collision_normal);
}

// Check ball (a) against each ball and returns whichever ball it is colliding with
std::pair<std::shared_ptr<Ball>,std::shared_ptr<Ball>> ballBallCollision(std::vector<std::shared_ptr<Ball>>& ball_instances, 
    std::vector<std::pair<std::shared_ptr<Ball>,std::shared_ptr<Ball>>>& ballball_collisions, 
    std::shared_ptr<Ball>& a)
{ 
    // Iterating through ball_instances : a is subject, b is ball in ball_instances
    for(auto& b : ball_instances) {
        if(&a == &b) continue; // Guard clause -- no ball will have collision with itself
        
        // If distance of A<->b <= radius a + radius b, then the balls are colliding
        if(Vector2Distance(a->position,b->position) <= (a->radius,b->radius)) {
            bool ifExists{};

            // Check if std::pair(b,a) doesn't already exist
            for(auto& x : ballball_collisions) {
                if(x == std::make_pair(b,a)) ifExists = true;
            }
            if(!ifExists) return std::make_pair(a,b);
        }
    }
    return std::make_pair(a,nullptr);
}

int main()
{
    InitWindow(WIDTH,HEIGHT,"physics-sim-2");
    SetTargetFPS(FRAMERATE);

    // !-- We're using std::shared_ptr so that std::vector has ownership of the Ball instances, meaning they are automatically
    //     deleted as the vector goes out of scope, but so that functions can duplicate the pointers.
    std::vector<std::shared_ptr<Ball>> ball_instances{};
    ball_instances.push_back(std::make_shared<Ball>(CLITERAL(Vector2){50.0f,50.0f}, 20.0f, RED));
    ball_instances.push_back(std::make_shared<Ball>(CLITERAL(Vector2){100.0f,50.0f}, 20.0f, RED));
    ball_instances.push_back(std::make_shared<Ball>(CLITERAL(Vector2){150.0f,50.0f}, 20.0f, RED));

    unsigned int ball_selection{};

    std::vector<std::pair<std::shared_ptr<Ball>,Vector2>> ballbound_collisions_to_occur{};
    std::vector<std::pair<std::shared_ptr<Ball>,std::shared_ptr<Ball>>> ballball_collisions_to_occur{};

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Add Force Downwards
/*         if(IsMouseButtonHeld(MOUSE_BUTTON_LEFT)) {
            DrawVector(ball_instances[ball_selection]->position)
        } else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            ball_instances[0]->addForce(CLITERAL(Vector2){0.0f,-1.0f});
        } */

        if(IsKeyPressed(KEY_SPACE)) {
            ball_instances[ball_selection]->color = RED;
            ball_selection++;
            if(ball_selection >= ball_instances.size()) ball_selection = 0;
        }
        ball_instances[ball_selection]->color = GREEN;

        ballbound_collisions_to_occur.clear();
        ballball_collisions_to_occur.clear();
        for(auto& ball : ball_instances) {
            // Update each Ball at the end of the frame
            ball->resolve(GetFrameTime());

            // Handle Ball<->Bound collisions
            ballbound_collisions_to_occur.push_back(ballBoundCollision(ball_instances, ball));

            // Handle Ball<->Ball collisions
            ballball_collisions_to_occur.push_back(ballBallCollision(ball_instances, ballball_collisions_to_occur, ball));
        }
        // Resolve collisions
        resolveCollisions(ballbound_collisions_to_occur);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}