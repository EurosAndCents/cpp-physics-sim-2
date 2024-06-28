#include "./objects/ball.h"
#include "./utility/vector_utility.h"

#include "./utility/constants.h"
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
        
        // We're multiplying normal by itself so we know which component of the resulting velocity (x or y) to affect with the coefficient of restitution
        Vector2 normal_pos{Vector2Multiply(normal,normal)};
        v = Vector2Multiply(v, Vector2Scale(normal_pos,RESTITUTION)); // e=v/u -> v=e*u

        // We're inverting the normal so we know which component of the resulting velocity to keep
        normal_pos = CLITERAL(Vector2){normal_pos.y,normal_pos.x};
        v = Vector2Add(v, Vector2Multiply(normal_pos, ball->velocity));
        
        ball->velocity = v; // Update velocity
    }
}

// Check ball (a) against each bound on the screen and return a std::pair(unique_ptr<Ball>&, Vector2 normal)
std::pair<std::shared_ptr<Ball>,Vector2> ballBoundCollision(std::vector<std::shared_ptr<Ball>>& ball_instances,
    std::shared_ptr<Ball>& a)
{
    Vector2 collision_normal{Vector2Zero()};

    // If outside of left bound or right bound
    if((a->position.x <= a->radius) || (a->position.x >= WIDTH-a->radius)) {
        collision_normal.x = (a->position.x <= a->radius) ? 1 : -1;
    }

    // If outside of top bound or bottom bound
    else if((a->position.y <= a->radius) || (a->position.y >= HEIGHT-a->radius)) {
        collision_normal.y = (a->position.y <= a->radius) ? 1 : -1;
    }

    std::cout << "normal of " << a << " | velo: (" << a->velocity.x << ", " << a->velocity.y << ")\n";

    return std::make_pair(a, collision_normal);
}

// // Check ball (a) against each ball and returns whichever ball it is colliding with
// std::pair<std::shared_ptr<Ball>,std::shared_ptr<Ball>> ballBallCollision(std::vector<std::shared_ptr<Ball>>& ball_instances, 
//     std::vector<std::pair<std::shared_ptr<Ball>,std::shared_ptr<Ball>>>& ballball_collisions, 
//     std::shared_ptr<Ball>& a)
// { 
//     // Iterating through ball_instances : a is subject, b is ball in ball_instances
//     for(auto& b : ball_instances) {
//         if(&a == &b) continue; // Guard clause -- no ball will have collision with itself
        
//         // If distance of A<->b <= radius a + radius b, then the balls are colliding
//         if(Vector2Distance(a->position,b->position) <= (a->radius,b->radius)) {
//             bool ifExists{};

//             // Check if std::pair(b,a) doesn't already exist
//             for(auto& x : ballball_collisions) {
//                 if(x == std::make_pair(b,a)) ifExists = true;
//             }
//             if(!ifExists) return std::make_pair(a,b);
//         }
//     }
//     return std::make_pair(a,nullptr);
// }

int main()
{
    InitWindow(WIDTH,HEIGHT,"physics-sim-2");
    SetTargetFPS(FRAMERATE);

    // !-- We're using std::shared_ptr so that std::vector has ownership of the Ball instances, meaning they are automatically
    //     deleted as the vector goes out of scope, but so that functions can duplicate the pointers.
    std::vector<std::shared_ptr<Ball>> ball_instances{};
    ball_instances.push_back(std::make_shared<Ball>(CLITERAL(Vector2){50.0f,50.0f}, 3.0f, RED));
    ball_instances.push_back(std::make_shared<Ball>(CLITERAL(Vector2){100.0f,50.0f}, 5.0f, RED));

    unsigned int ball_selection{};

    std::vector<std::pair<std::shared_ptr<Ball>,Vector2>> ballbound_collisions_to_occur{};
    std::vector<std::pair<std::shared_ptr<Ball>,std::shared_ptr<Ball>>> ballball_collisions_to_occur{};

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Add impulse
        Vector2 impulse_to_apply{getVectorDirection(GetMousePosition(),ball_instances[ball_selection]->position)};
        impulse_to_apply = Vector2Scale(impulse_to_apply, 500.0f);
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            drawVector(ball_instances[ball_selection]->position, GetMousePosition(), GREEN, 50.0f); // Preview impulse
        } else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            ball_instances[ball_selection]->addImpulse(impulse_to_apply,true); // Add impulse
        }

        // Change Selection on Space
        if(IsKeyPressed(KEY_SPACE)) {
            ball_instances[ball_selection]->color = RED; // Reset unselected Ball to red
            ball_selection++;
            if(ball_selection >= ball_instances.size()) ball_selection = 0; // Wrap around if outside of vector size
        }
        ball_instances[ball_selection]->color = GREEN; // Set selected Ball to green

        ballbound_collisions_to_occur.clear();
        ballball_collisions_to_occur.clear();
        for(auto& ball : ball_instances) {
            // Update each Ball at the end of the frame
            ball->resolve(GetFrameTime());

            // Handle Ball<->Bound collisions
            ballbound_collisions_to_occur.push_back(ballBoundCollision(ball_instances, ball));

            // Handle Ball<->Ball collisions
            // // ballball_collisions_to_occur.push_back(ballBallCollision(ball_instances, ballball_collisions_to_occur, ball));

            //std::cout << ball->velocity.x << ", " << ball->velocity.y << '\n';
        }
        // Resolve collisions
        resolveCollisions(ballbound_collisions_to_occur);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}