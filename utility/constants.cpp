#include "constants.h"

#include INCLUDE_RAYLIB

// Provides global variables for this project.
const int WIDTH{900};
const int HEIGHT{600};
const int FRAMERATE{60};

Vector2 GRAVITY{0.0f,9.81f}; // pixels/second^2
float RESTITUTION{0.6f}; // -e=v/u