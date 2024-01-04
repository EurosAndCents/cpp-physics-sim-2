#include "ball.h"

#include "constants.h"
#include INCLUDE_RAYLIB
#include INCLUDE_RAYMATH

Ball::Ball(Vector2 pos, float kg) : 
    position{pos}, velocity{}, acceleration{}, force{},
    mass{kg}
{
    this->addForce(GRAVITY); // force{} could be initialized with GRAVITY, but we're doing it this way as the logic is easier to follow
}

void Ball::resolve(float delta_time) 
{
    // !-- We're using t=deltaTime (time between frames) as this acceleration/velocity is applied each frame, and is stored in the form
    //     per second or per second^2

    // Calculate Acceleration (f=ma -> a=f/m)
    this->acceleration = {Vector2Scale(this->force, 1/this->mass)}; // We're multiplying by the reciprocal of a scalar quantity which is the same as division

    // Calculate Velocity (v=u+at)
    this->velocity = Vector2Add(this->velocity, this->acceleration);
    Vector2 velocity_deltatime = Vector2Scale(this->velocity, delta_time); // We're storing this in a different variable as it is stored as per second in Ball

    // Calculate Position (s=ut+0.5at^2 -> s=vt as acceleration has already been resolved)
    this->position = Vector2Add(this->position, Vector2Scale(velocity_deltatime, delta_time));

    // Draw Ball
    DrawCircleV(this->position, 7.5f, RED);
}

void Ball::addForce(Vector2 v)
{
    this->force = Vector2Add(this->force, v);
}

void Ball::addImpulse(Vector2 v, bool ignoreVelo=false)
{
    if(ignoreVelo) {
        this->velocity = v;
    } else {
        this->velocity = Vector2Add(this->velocity, v);
    }
}

// GETTERS
Vector2 Ball::getPosition() const {return this->position;}
Vector2 Ball::getVelocity() const {return this->velocity;}
Vector2 Ball::getAcceleration() const {return this->acceleration;}
Vector2 Ball::getForce() const {return this->force;}
float Ball::getMass() const {return this->mass;}