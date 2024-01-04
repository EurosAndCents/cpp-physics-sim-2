#pragma once

#include "constants.h"
#include INCLUDE_RAYLIB

class Ball
{
    public:
        Vector2 position; // pixels
        Vector2 velocity; // pixels/second
        Vector2 acceleration; // pixels/second^2
        Vector2 force; // N (kg pixels/second^2)
        float mass; // kg
        float radius; // pixels
        
        Ball(Vector2 pos, float mass);

        // Called each frame to calculate the acceleration (from forces), the velocity (from acceleration), and the position (from velocity)
        void resolve(float delta_time);
        void addForce(Vector2 v);
        void addImpulse(Vector2 v, bool ignoreVelo); // ignoreVelo=true will set the velocity rather than add, similar to the handling of 2D platformers
};