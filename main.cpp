#include "./objects/ball.h"

#include "constants.h"
#include INCLUDE_RAYLIB

#include <iostream>
#include <vector>
#include <memory>

int main()
{
    InitWindow(WIDTH,HEIGHT,"physics-sim-2");
    SetTargetFPS(FRAMERATE);

    // !-- We're using std::unique_ptr so that std::vector has ownership of the Ball instances, meaning they are automatically
    //     deleted as the vector goes out of scope.
    std::vector<std::unique_ptr<Ball>> ball_instances{};
    ball_instances.push_back(std::make_unique<Ball>(CLITERAL(Vector2){50.0f,50.0f}, 1.0f));

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Add Force Downwards
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            ball_instances[0]->addForce(CLITERAL(Vector2){0.0f,5.0f});
        } else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            ball_instances[0]->addForce(CLITERAL(Vector2){0.0f,-5.0f});
        }
        
        // Add Force Upwards
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            ball_instances[0]->addForce(CLITERAL(Vector2){0.0f,-5.0f});
        } else if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
            ball_instances[0]->addForce(CLITERAL(Vector2){0.0f,5.0f});
        }

        // Handle Ball<->Bound Collisions

        // Handle Ball<->Ball Collisions

        // Update each Ball at the end of the frame
        for(auto& ball : ball_instances) {
            ball->resolve(GetFrameTime());
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}