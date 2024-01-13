#pragma once

#include "constants.h"
#include INCLUDE_RAYLIB
#include INCLUDE_RAYMATH

// Gets direction of a displacement vector (< 1,1)
inline Vector2 getVectorDirection(Vector2 v, Vector2 u=Vector2Zero())
{
    // Get resulting displacement vector
    Vector2 s = Vector2Subtract(v,u);

    // Create a normalized vector (vector indicating direction) by dividing each component by magnitude
    float m = sqrt(s.x*s.x + s.y*s.y);
    Vector2 result = Vector2Divide(s,CLITERAL(Vector2){m,m});

    return result; // Returns vector of direction (< 1,1) to be multiplied with magnitude
}

// Draw a displacement vector from a point
inline void drawVector(Vector2 pos, Vector2 v, Color color=BLACK, float m=0.0f) 
{
    Vector2 s{};
    if(m==0.0f) {
        s = Vector2Add(pos, v); // Position after v applied
    } else {
        Vector2 s_dir = Vector2Scale(getVectorDirection(v,pos),m);
        s = Vector2Add(pos, s_dir); // Position after v applied, with a length of m
    }

    DrawLineV(pos, s, color);
}