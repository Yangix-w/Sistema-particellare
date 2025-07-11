#include "particle.h"


Particle::Particle() 
    : position(0.0f), velocity(0.0f), color(1.0f, 1.0f, 1.0f, 1.0f), 
      size(1.0f), life(1.0f), cameradistance(0.0f) {}

Particle::Particle(glm::vec3 position, glm::vec3 velocity, glm::vec4 color,
                   float size, float life)
    : position(position), velocity(velocity), color(color),
      size(size), life(life), cameradistance(0.0f) {}

Particle::~Particle() {}

Particle::Particle(const Particle &other) 
    : position(other.position), velocity(other.velocity), color(other.color), 
      size(other.size), life(other.life),
      cameradistance(other.cameradistance) {}

Particle &Particle::operator=(const Particle &other) {
    if (this != &other) {
        position = other.position;
        velocity = other.velocity;
        color = other.color;
        size = other.size;
        life = other.life;
        cameradistance = other.cameradistance;
    }
    return *this;
}

bool Particle::operator<(Particle &other){
    return this->cameradistance > other.cameradistance;
}