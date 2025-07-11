#include "particleSystem.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "glm/gtx/norm.hpp"

// Definizioni delle variabili globali
int ParticlesCount = 0;
float delta = 0.016f; // 60 FPS circa

ParticleSystem::ParticleSystem(){
    // Inizializza il generatore di numeri casuali
    srand(static_cast<unsigned int>(time(nullptr)));
}

ParticleSystem::~ParticleSystem(){
    // Dealloca le particelle
    for (int i = 0; i < MAX_PARTICLES; ++i){
        delete particles[i];
        particles[i] = nullptr;
    }
}

ParticleSystem::ParticleSystem(const ParticleSystem &other){
    // Copia le particelle dall'oggetto passato
    for (int i = 0; i < MAX_PARTICLES; ++i){
        particles[i] = new Particle(*other.particles[i]);
    }
}

ParticleSystem &ParticleSystem::operator=(const ParticleSystem &other){
    if (this != &other){
        // Dealloca le particelle esistenti
        for (int i = 0; i < MAX_PARTICLES; ++i){
            delete particles[i];
            particles[i] = nullptr;
        }

        // Copia le particelle dall'oggetto passato
        for (int i = 0; i < MAX_PARTICLES; ++i){
            particles[i] = new Particle(*other.particles[i]);
        }
    }
    return *this;
}

bool ParticleSystem::loadTexture(const std::string& texturePath) {
    if (particle_texture.load(texturePath)) {
        return true;
    } else {
        return false;
    }
}

void ParticleSystem::init(){
    if (!arrayInit){
        for (int i = 0; i < MAX_PARTICLES; ++i){
            particles[i] = nullptr;
            particles[i] = new Particle();
        }
        arrayInit = true; // array inizializzato
    }

    // billboard
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };

    // Create and bind the VAO
    glGenVertexArrays(1, &(_VAO));
    glBindVertexArray(_VAO);

    // The VBO containing the 4 vertices of the billboard
    GLuint billboard_vertex_buffer;
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(
        0,        // attribute. No particular reason for 0, but must match the layout in the shader.
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );

    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(
        1,        // attribute. No particular reason for 1, but must match the layout in the shader.
        4,        // size : x + y + z + size => 4
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void *)0 // array buffer offset
    );

    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(
        2,                // attribute. No particular reason for 2, but must match the layout in the shader.
        4,                // size : r + g + b + a => 4
        GL_UNSIGNED_BYTE, // type
        GL_TRUE,          // normalized? *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
        0,                // stride
        (void *)0         // array buffer offset
    );

    initialized = true; // Sistema inizializzato

    glBindVertexArray(0);
}

void ParticleSystem::fireRender(const glm::vec3 &cameraPosition){
    // Aggiorna le particelle ogni frame
    ParticlesCount = 0; // viene resettato ad ogni frame

    for (int i = 0; i < MAX_PARTICLES; i++){
        Particle *p = particles[i];

        if (p->life > 0.0f){
            // descremento la vita della particella
            p->life -= delta;
            if (p->life > 0.0f){

                // Forza verso l'alto controllata - le particelle salgono dolcemente
                float upwardForce = 0.8f;
                p->velocity += glm::vec3(0.0f, upwardForce, 0.0f) * (float)delta;

                // Limita la velocità verticale massima per evitare che salgano troppo veloce
                float maxUpwardSpeed = 2.5f;
                if (p->velocity.y > maxUpwardSpeed){
                    p->velocity.y = maxUpwardSpeed;
                }

                // Aggiunge turbolenza/oscillazione alle fiamme (random)
                float turbulence = 0.5f;
                p->velocity.x += ((rand() % 200 - 100) / 100.0f) * turbulence * delta; // -1.0 a 1.0
                p->velocity.z += ((rand() % 200 - 100) / 100.0f) * turbulence * delta; // -1.0 a 1.0

                // Rallenta la velocità nel tempo (resistenza dell'aria)
                p->velocity *= 0.98f; // 2%

                p->position += p->velocity * (float)delta;
                p->cameradistance = glm::length2(p->position - cameraPosition);

                // Aggiorno il GPU buffer
                g_particule_position_size_data[4 * ParticlesCount + 0] = p->position.x;
                g_particule_position_size_data[4 * ParticlesCount + 1] = p->position.y;
                g_particule_position_size_data[4 * ParticlesCount + 2] = p->position.z;
                g_particule_position_size_data[4 * ParticlesCount + 3] = p->size;

                // Converto i colori da float (0.0-1.0) a byte (0-255)
                g_particule_color_data[4 * ParticlesCount + 0] = (GLubyte)(p->color.r * 255.0f);
                g_particule_color_data[4 * ParticlesCount + 1] = (GLubyte)(p->color.g * 255.0f);
                g_particule_color_data[4 * ParticlesCount + 2] = (GLubyte)(p->color.b * 255.0f);
                g_particule_color_data[4 * ParticlesCount + 3] = (GLubyte)(p->color.a * 255.0f);

                ParticlesCount++;
            }
            else{
                // uccido le particelle morte e sortParticle() li metterà alla fine dell'array
                killParticle(i);
            }
        }
    }

    // ordino le particelle per rendering corretto
    sortParticles();
}

void ParticleSystem::explosionRender(const glm::vec3 &cameraPosition){
    ParticlesCount = 0;
    for (int i = 0; i < MAX_PARTICLES; i++){

        Particle *p = particles[i];

        if (p->life > 0.0f){

            p->life -= delta;
            if (p->life > 0.0f){

                // Simulazione semplice della gravità
                p->velocity += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
                p->position += p->velocity * (float)delta;
                p->cameradistance = glm::length2(p->position - cameraPosition);

                g_particule_position_size_data[4 * ParticlesCount + 0] = p->position.x;
                g_particule_position_size_data[4 * ParticlesCount + 1] = p->position.y;
                g_particule_position_size_data[4 * ParticlesCount + 2] = p->position.z;
                g_particule_position_size_data[4 * ParticlesCount + 3] = p->size;

                g_particule_color_data[4 * ParticlesCount + 0] = (GLubyte)(p->color.r * 255.0f);
                g_particule_color_data[4 * ParticlesCount + 1] = (GLubyte)(p->color.g * 255.0f);
                g_particule_color_data[4 * ParticlesCount + 2] = (GLubyte)(p->color.b * 255.0f);
                g_particule_color_data[4 * ParticlesCount + 3] = (GLubyte)(p->color.a * 255.0f);

                ParticlesCount++;
            }
            else{
                killParticle(i);
            }
        }
    }

    sortParticles();
}

void ParticleSystem::rainRender(const glm::vec3 &cameraPosition){
    ParticlesCount = 0;
    for (int i = 0; i < MAX_PARTICLES; i++){
        Particle *p = particles[i];

        if (p->life > 0.0f){

            p->life -= delta;
            if (p->life > 0.0f){
                // Gravità molto forte verso il basso
                p->velocity += glm::vec3(0.0f, -35.0f, 0.0f) * (float)delta;
                
                // Leggero attrito dell'aria per le gocce
                p->velocity *= 0.99f; // 1%
                
                // Aggiunge leggera turbolenza del vento (random)
                float windTurbulence = 0.1f;
                p->velocity.x += ((rand() % 200 - 100) / 100.0f) * windTurbulence * delta; // -1.0 a 1.0
                p->velocity.z += ((rand() % 200 - 100) / 100.0f) * windTurbulence * delta; // -1.0 a 1.0
                
                p->position += p->velocity * (float)delta;
                p->cameradistance = glm::length2(p->position - cameraPosition);

                // Elimina le gocce che toccano terra (faccendo finta che il terreno è a -5.0)
                if (p->position.y < -5.0f) {
                    killParticle(i);
                    continue;
                }

                g_particule_position_size_data[4 * ParticlesCount + 0] = p->position.x;
                g_particule_position_size_data[4 * ParticlesCount + 1] = p->position.y;
                g_particule_position_size_data[4 * ParticlesCount + 2] = p->position.z;
                g_particule_position_size_data[4 * ParticlesCount + 3] = p->size;

                g_particule_color_data[4 * ParticlesCount + 0] = (GLubyte)(p->color.r * 255.0f);
                g_particule_color_data[4 * ParticlesCount + 1] = (GLubyte)(p->color.g * 255.0f);
                g_particule_color_data[4 * ParticlesCount + 2] = (GLubyte)(p->color.b * 255.0f);
                g_particule_color_data[4 * ParticlesCount + 3] = (GLubyte)(p->color.a * 255.0f);

                ParticlesCount++;
            }
            else{
                killParticle(i);
            }
        }
    }

    sortParticles();
}

void ParticleSystem::smokeRender(const glm::vec3 &cameraPosition){
    ParticlesCount = 0;
    for (int i = 0; i < MAX_PARTICLES; i++){
        Particle *p = particles[i];

        if (p->life > 0.0f){

            p->life -= delta;
            if (p->life > 0.0f){
                // Il fumo sale dolcemente
                float upwardForce = 1.2f;
                p->velocity += glm::vec3(0.0f, upwardForce, 0.0f) * (float)delta;

                // Espansione radiale: il fumo si allarga verso l'esterno nel tempo
                float ageRatio = 1.0f - (p->life / 8.0f); // Ratio dell'età (0 = nuovo, 1 = vecchio)
                float expansionForce = ageRatio * 0.8f; // Aumenta l'espansione con l'età
                
                // Calcola direzione radiale dal centro (assumendo centro a origine)
                glm::vec3 radialDirection = glm::normalize(glm::vec3(p->position.x, 0.0f, p->position.z));
                if (glm::length(radialDirection) < 0.1f) {
                    // Se troppo vicino al centro, usa una direzione casuale
                    radialDirection = glm::vec3(
                        (rand() % 200 - 100) / 100.0f,  // -1.0 a 1.0
                        0.0f,
                        (rand() % 200 - 100) / 100.0f   // -1.0 a 1.0
                    );
                    radialDirection = glm::normalize(radialDirection);
                }
                
                // Applica forza di espansione radiale
                p->velocity += radialDirection * expansionForce * (float)delta;

                // Turbolenza del fumo
                float turbulence = 0.8f;
                p->velocity.x += ((rand() % 200 - 100) / 100.0f) * turbulence * delta; // -1.0 a 1.0
                p->velocity.z += ((rand() % 200 - 100) / 100.0f) * turbulence * delta; // -1.0 a 1.0

                // Resistenza dell'aria
                p->velocity *= 0.96f; // 4%

                // Espansione del fumo nel tempo: le particelle diventano più grandi
                p->size += ageRatio * 0.05f * delta;      // Crescita graduale

                // Limita dimensione massima
                if (p->size > 1.5f){
                    p->size = 1.5f;
                }

                // Effetto dissolvenza: il fumo diventa più trasparente nel tempo
                float maxLife = 10.0f;
                float lifeRatio = p->life / maxLife;
                p->color.a *= (0.3f + lifeRatio * 0.7f); // Diventa più trasparente con l'età

                // Per evitare valori negativi
                if (p->color.a < 0.1f){
                    p->color.a = 0.1f;
                }

                p->position += p->velocity * (float)delta;
                p->cameradistance = glm::length2(p->position - cameraPosition);

                g_particule_position_size_data[4 * ParticlesCount + 0] = p->position.x;
                g_particule_position_size_data[4 * ParticlesCount + 1] = p->position.y;
                g_particule_position_size_data[4 * ParticlesCount + 2] = p->position.z;
                g_particule_position_size_data[4 * ParticlesCount + 3] = p->size;

                g_particule_color_data[4 * ParticlesCount + 0] = (GLubyte)(p->color.r * 255.0f);
                g_particule_color_data[4 * ParticlesCount + 1] = (GLubyte)(p->color.g * 255.0f);
                g_particule_color_data[4 * ParticlesCount + 2] = (GLubyte)(p->color.b * 255.0f);
                g_particule_color_data[4 * ParticlesCount + 3] = (GLubyte)(p->color.a * 255.0f);

                ParticlesCount++;
            }
            else{
                killParticle(i);
            }
        }
    }

    sortParticles();
}

void ParticleSystem::render(const int rendereTyper, const glm::vec3 &cameraPosition){
    if(initialized == false){
        init();
    }

    switch (rendereTyper) {
        case 0: // Fuoco
            fireRender(cameraPosition);
            break;
        case 1: // Esplosione
            explosionRender(cameraPosition);
            break;
        case 2: // Pioggia
            rainRender(cameraPosition);
            break;
        case 3: // Fumo
            smokeRender(cameraPosition);
            break;
    }

    glBindVertexArray(_VAO);
    // Aggiorna i buffer OpenGL con i nuovi dati delle particelle
    if (ParticlesCount > 0) {
        // Update position buffer
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

        // Update color buffer
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);
    }


    // These functions are specific to glDrawArrays*Instanced*.
    // The first parameter is the attribute buffer we're talking about.
    // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1
    glVertexAttribDivisor(2, 1); // color : one per quad -> 1

    // Draw the particules !
    // This draws many times a small triangle_strip (which looks like a quad).
    // This is equivalent to :
    // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
    // but faster.
    if (ParticlesCount > 0) {
        // Bind della texture per le particelle se disponibile
        if (particle_texture.is_valid()) {
            particle_texture.bind(0); // Bind alla texture unit 0
        }
        
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);
    }

    glBindVertexArray(0);
}

void ParticleSystem::sortParticles(){
    std::sort(&particles[0], &particles[ParticlesCount]);
}

int ParticleSystem::findUnusedParticle(){

    for (int i = lastUsed; i < MAX_PARTICLES; i++){
        if (particles[i]->life <= 0){
            lastUsed = i;
            return i;
        }
    }

    for (int i = 0; i < lastUsed; i++){
        if (particles[i]->life <= 0){
            lastUsed = i;
            return i;
        }
    }

    return -1;
}

void ParticleSystem::killParticle(int index){
    if (index < 0 || index >= MAX_PARTICLES)
        return;

    // Reset la particella ai valori di default (particella "morta")
    particles[index]->position = glm::vec3(0.0f);
    particles[index]->velocity = glm::vec3(0.0f);
    particles[index]->color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f); // Alpha = 0
    particles[index]->size = 0.0f;
    particles[index]->life = 0.0f; // Vita = 0 (morta)
    particles[index]->cameradistance = -1.0f;
}

void ParticleSystem::spawnParticle(glm::vec3 pos, glm::vec3 vel, glm::vec4 col,
                                   float particleSize, float particleLife){
    int deadIndex = findUnusedParticle();
    if (deadIndex != -1){
        // Riutilizza una particella morta
        particles[deadIndex]->position = pos;
        particles[deadIndex]->velocity = vel;
        particles[deadIndex]->color = col;
        particles[deadIndex]->size = particleSize;
        particles[deadIndex]->life = particleLife;
        particles[deadIndex]->cameradistance = 0.0f;
    } else {
        // Se non ci sono particelle morte, forza la morte della più vecchia
        static int forceKillIndex = 0;
        killParticle(forceKillIndex);
        
        // Usa quella appena liberata
        particles[forceKillIndex]->position = pos;
        particles[forceKillIndex]->velocity = vel;
        particles[forceKillIndex]->color = col;
        particles[forceKillIndex]->size = particleSize;
        particles[forceKillIndex]->life = particleLife;
        particles[forceKillIndex]->cameradistance = 0.0f;
        
        forceKillIndex = (forceKillIndex + 1) % MAX_PARTICLES; // Cicla attraverso tutte le particelle
    }
}

void ParticleSystem::createFireSources(glm::vec3 basePos, float intensity){
    static float fireTime = 0.0f;
    fireTime += delta;

    // Array di posizioni relative per creare un fuoco concentrato al centro
    glm::vec3 firePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),  // Centro - principale
        glm::vec3(0.3f, 0.0f, 0.0f),  // Destra vicina
        glm::vec3(-0.3f, 0.0f, 0.0f), // Sinistra vicina
        glm::vec3(0.0f, 0.0f, 0.3f),  // Dietro vicino
        glm::vec3(0.0f, 0.0f, -0.3f), // Davanti vicino
        glm::vec3(0.2f, 0.0f, 0.2f),  // Diagonale piccola
        glm::vec3(-0.2f, 0.0f, -0.2f) // Diagonale opposta piccola
    };

    // Intensità relative per ogni sorgente (centro più forte)
    float sourceWeights[] = {
        1.0f, // Centro - intensità massima
        0.6f, // Laterali - intensità media
        0.6f,
        0.6f,
        0.6f,
        0.4f, // Diagonali - intensità ridotta
        0.4f};

    int numSources = sizeof(firePositions) / sizeof(firePositions[0]);

    // Variazione temporale per effetto più dinamico
    float timeVariation = sin(fireTime * 2.0f) * 0.3f + 0.7f; // 0.4 - 1.0

    for (int source = 0; source < numSources; source++){
        glm::vec3 currentPos = basePos + firePositions[source];

        // Intensità basata sul peso della sorgente e distanza dal centro
        float baseIntensity = intensity * sourceWeights[source] * timeVariation;

        // Le sorgenti centrali emettono più particelle
        float emissionRate = baseIntensity * (source == 0 ? 40.0f : 20.0f); // Centro più attivo

        static float timers[7] = {0.0f}; // Un timer per ogni sorgente
        timers[source] += delta;

        float timePerParticle = 1.0f / emissionRate;
        int particlesThisFrame = 0;
        const int maxPerSource = (source == 0 ? 8 : 4); // Centro può emettere più particelle

        while (timers[source] >= timePerParticle && particlesThisFrame < maxPerSource){
            // tipo di particella basato sulla posizione
            float particleType = (rand() % 100) / 100.0f;

            glm::vec3 velocity;
            glm::vec4 color;
            float size;
            float life;

            if (source == 0){
                // Sorgente centrale, più concentrate
                velocity = glm::vec3(
                    (rand() % 80 - 40) / 200.0f,                    // Movimento laterale molto ridotto
                    3.5f + (rand() % 250) / 100.0f * baseIntensity, // Forte movimento verso l'alto
                    (rand() % 80 - 40) / 200.0f                     // Movimento laterale molto ridotto
                );

                // Colori più caldi e brillanti per il centro
                float heat = (rand() % 100) / 100.0f;
                if (heat > 0.7f){
                    color = glm::vec4(1.0f, 0.98f, 0.8f, 0.98f); // Bianco-giallo brillante
                }
                else if (heat > 0.4f){
                    color = glm::vec4(1.0f, 0.85f, 0.3f, 0.95f); // Giallo-arancione
                }
                else{
                    color = glm::vec4(1.0f, 0.7f, 0.2f, 0.9f); // Arancione
                }

                size = (0.3f + (rand() % 25) / 100.0f) * baseIntensity * 1.0f;
                life = (1.0f + (rand() % 60) / 100.0f) * 0.8f; // Applicato moltiplicatore
            }
            else if (particleType < 0.8f){
                // Sorgente laterale
                // Velocità che tende verso il centro
                glm::vec3 toCenter = glm::normalize(-firePositions[source]);
                float centerAttraction = 0.3f;

                velocity = glm::vec3(
                    (rand() % 100 - 50) / 120.0f + toCenter.x * centerAttraction,
                    2.2f + (rand() % 180) / 100.0f * baseIntensity,
                    (rand() % 100 - 50) / 120.0f + toCenter.z * centerAttraction);

                // Colori leggermente più freddi per i lati
                color = glm::vec4(
                    0.95f + (rand() % 5) / 100.0f,  // R
                    0.6f + (rand() % 25) / 100.0f,  // G
                    0.15f + (rand() % 15) / 100.0f, // B
                    0.85f);

                size = (0.2f + (rand() % 20) / 100.0f) * baseIntensity * 1.0f;
                life = (0.9f + (rand() % 50) / 100.0f) * 0.8f; // Applicato moltiplicatore
            }
            else{
                // Scintille
                float sparkAngle = (rand() % 360) * 3.14159f / 180.0f;
                float sparkForce = 1.2f + (rand() % 150) / 100.0f;

                // Forza le scintille verso il centro se sono troppo lontane
                glm::vec3 toCenter = glm::normalize(-firePositions[source]) * 0.4f;

                velocity = glm::vec3(
                    cos(sparkAngle) * sparkForce * 0.6f + toCenter.x,
                    sparkForce + (rand() % 180) / 100.0f,
                    sin(sparkAngle) * sparkForce * 0.6f + toCenter.z);

                // Scintille giallo-arancione brillanti
                color = glm::vec4(
                    1.0f,                           // R
                    0.4f + (rand() % 35) / 100.0f,  // G
                    0.08f + (rand() % 12) / 100.0f, // B
                    0.95f);

                size = (0.06f + (rand() % 10) / 100.0f) * 1.0f;
                life = (0.6f + (rand() % 30) / 100.0f) * 0.8f; // Applicato moltiplicatore
            }

            // Variazione minima nella posizione: più concentrato
            glm::vec3 finalPos = currentPos + glm::vec3(
                                                  (rand() % 40 - 20) / 300.0f, // Variazione ridotta
                                                  (rand() % 20) / 300.0f,      // Variazione ridotta
                                                  (rand() % 40 - 20) / 300.0f  // Variazione ridotta
                                              );

            spawnParticle(finalPos, velocity, color, size, life);

            timers[source] -= timePerParticle;
            particlesThisFrame++;
        }
    }
}

void ParticleSystem::createExplosion(glm::vec3 center, int numParticles, float force){
    
    // Palla di fuoco centrale (30% delle particelle)
    int coreParticles = numParticles * 0.3f;
    for (int i = 0; i < coreParticles; i++){
        // Direzione casuale sferica con bias verso l'esterno
        float theta = (rand() % 360) * 3.14159f / 180.0f;
        float phi = (rand() % 120 + 30) * 3.14159f / 180.0f; // Più orizzontale
        
        glm::vec3 direction = glm::vec3(
            sin(phi) * cos(theta),
            cos(phi) * 0.7f, // Riduce movimento verticale
            sin(phi) * sin(theta));
        
        // Velocità della palla di fuoco: esplosione rapida iniziale
        float speedVariation = 0.8f + (rand() % 60) / 100.0f; // 0.8-1.4
        glm::vec3 velocity = direction * force * speedVariation;
        
        // Colori della palla di fuoco: bianco/giallo brillante
        float heat = (rand() % 100) / 100.0f;
        glm::vec4 coreColor;
        if (heat > 0.8f) {
            coreColor = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f); // Bianco caldo
        } else if (heat > 0.5f) {
            coreColor = glm::vec4(1.0f, 0.95f, 0.6f, 0.98f); // Giallo brillante
        } else {
            coreColor = glm::vec4(1.0f, 0.8f, 0.4f, 0.95f); // Arancione caldo
        }
        
        float size = 0.4f + (rand() % 50) / 100.0f; // Particelle grandi per palla di fuoco
        float life = 1.8f + (rand() % 80) / 100.0f; // Vita lunga per effetto persistente
        
        spawnParticle(center, velocity, coreColor, size, life);
    }
    
    // Detriti (40% delle particelle)
    int debrisParticles = numParticles * 0.4f;
    for (int i = 0; i < debrisParticles; i++){
        // Direzione più casuale per detriti
        float theta = (rand() % 360) * 3.14159f / 180.0f;
        float phi = (rand() % 180) * 3.14159f / 180.0f;
        
        glm::vec3 direction = glm::vec3(
            sin(phi) * cos(theta),
            cos(phi),
            sin(phi) * sin(theta));
        
        // Velocità variabile per detriti, alcuni veloci altri lenti
        float speedMultiplier = 0.3f + (rand() % 150) / 100.0f; // 0.3-1.8
        glm::vec3 velocity = direction * force * speedMultiplier;
        
        // Aggiunge gravità artificiale ai detriti
        velocity.y += (rand() % 200 - 100) / 100.0f; // -1.0 a +1.0 extra verticale
        
        // Colori detriti, dal rosso al Marrore
        float debrisHeat = (rand() % 100) / 100.0f;
        glm::vec4 debrisColor;
        if (debrisHeat > 0.7f) {
            debrisColor = glm::vec4(1.0f, 0.4f, 0.1f, 0.9f); // Rosso arancione
        } else if (debrisHeat > 0.4f) {
            debrisColor = glm::vec4(0.8f, 0.2f, 0.05f, 0.8f); // Rosso scuro
        } else {
            debrisColor = glm::vec4(0.4f, 0.1f, 0.0f, 0.7f); // Marrone scuro
        }
        
        float size = 0.15f + (rand() % 30) / 100.0f; // Detriti più piccoli
        float life = 2.5f + (rand() % 150) / 100.0f; // Vita varia per detriti
        
        spawnParticle(center, velocity, debrisColor, size, life);
    }
    
    // Cenere (30% delle particelle)
    int smokeParticles = numParticles * 0.3f;
    for (int i = 0; i < smokeParticles; i++){
        // Direzione prevalentemente verso l'alto per cenere
        float theta = (rand() % 360) * 3.14159f / 180.0f;
        float phi = (rand() % 60 + 60) * 3.14159f / 180.0f; // Bias verso l'alto
        
        glm::vec3 direction = glm::vec3(
            sin(phi) * cos(theta) * 0.5f, // Movimento laterale ridotto
            cos(phi) + 0.5f, // Spinta verso l'alto
            sin(phi) * sin(theta) * 0.5f);
        
        // Velocità lenta per cenere
        float smokeSpeed = 0.2f + (rand() % 60) / 100.0f; // 0.2-0.8
        glm::vec3 velocity = direction * force * smokeSpeed;
        
        // Colori cenere, dal grigio scuro al grigio chiaro
        float smokeIntensity = (rand() % 60 + 20) / 100.0f; // 0.2-0.8
        glm::vec4 smokeColor = glm::vec4(
            smokeIntensity, 
            smokeIntensity, 
            smokeIntensity, 
            0.6f - (rand() % 30) / 100.0f // Alpha variabile 0.3-0.6
        );
        
        float size = 0.3f + (rand() % 70) / 100.0f; // cenere grande
        float life = 4.0f + (rand() % 200) / 100.0f; // cenere che persiste a lungo
        
        spawnParticle(center, velocity, smokeColor, size, life);
    }
}

void ParticleSystem::createRain(const glm::vec3 areaCenter, float areaSize, float intensity){
    
    static float rainTimer = 0.0f;
    rainTimer += delta;
    
    // Calcola il numero di gocce di pioggia da creare basandosi sull'intensità
    float rainRate = intensity * 150.0f; // Particelle al secondo
    float timePerDrop = 1.0f / rainRate;
    
    // Limita il numero massimo di gocce per frame per mantenere performance
    int maxDropsPerFrame = (int)(intensity * 20.0f);
    int dropsCreatedThisFrame = 0;
    
    while (rainTimer >= timePerDrop && dropsCreatedThisFrame < maxDropsPerFrame){
        // Posizione casuale nell'area
        // Crea gocce in un'area quadrata sopra il centro
        glm::vec3 dropPosition = glm::vec3(
            areaCenter.x + (rand() % (int)(areaSize * 200) - areaSize * 100) / 100.0f, // X casuale nell'area
            areaCenter.y + 8.0f + (rand() % 300) / 100.0f,                            // Y alta sopra l'area (8-11 unità)
            areaCenter.z + (rand() % (int)(areaSize * 200) - areaSize * 100) / 100.0f  // Z casuale nell'area
        );
        
        // Velocità prevalentemente verso il basso con leggera variazione laterale per vento
        glm::vec3 rainVelocity = glm::vec3(
            (rand() % 40 - 20) / 100.0f * intensity,    // Leggero movimento laterale (vento)
            -6.0f - (rand() % 200) / 100.0f * intensity, // Velocità verso il basso (più intensa = più veloce)
            (rand() % 30 - 15) / 100.0f * intensity     // Leggero movimento laterale Z
        );

        // Colore blu-trasparente per l'acqua
        float transparency = 0.6f + (rand() % 40) / 100.0f; // 0.6-1.0
        glm::vec4 rainColor = glm::vec4(
            0.7f + (rand() % 30) / 100.0f,  // R: 0.7-1.0 (leggero bianco)
            0.8f + (rand() % 20) / 100.0f,  // G: 0.8-1.0 (leggero bianco)
            1.0f,                           // B: 1.0 (blu pieno)
            transparency                    // A: trasparenza variabile
        );
        
        // Dimensione piccola per gocce di pioggia
        float dropSize = 0.02f + (rand() % 8) / 10.0f;
        
        // Vita breve, le gocce scompaiono rapidamente quando toccano terra
        float dropLife = 1.5f + (rand() % 100) / 100.0f; 
        
        spawnParticle(dropPosition, rainVelocity, rainColor, dropSize, dropLife);
        
        rainTimer -= timePerDrop;
        dropsCreatedThisFrame++;
    }
    
}

void ParticleSystem::createSmoke(glm::vec3 sourcePos, float intensity){
    
    static float smokeTimer = 0.0f;
    smokeTimer += delta;
    
    // Calcola il numero di particelle di fumo da creare basandosi sull'intensità
    float smokeRate = intensity * 80.0f;
    float timePerPuff = 1.0f / smokeRate;
    
    // Limita il numero massimo di sbuffi per frame per mantenere performance
    int maxPuffsPerFrame = (int)(intensity * 12.0f);
    int puffsCreatedThisFrame = 0;
    
    while (smokeTimer >= timePerPuff && puffsCreatedThisFrame < maxPuffsPerFrame){
        
        // Crea fumo in un piccolo cerchio attorno alla sorgente usando coordinate cartesiane
        float randomX = (rand() % 200 - 100) / 100.0f; // -1.0 a 1.0
        float randomZ = (rand() % 200 - 100) / 100.0f; // -1.0 a 1.0
        float radius = (rand() % 50) / 100.0f; // Raggio fino a 0.5 unità
        
        // Normalizza e applica il raggio
        glm::vec3 randomDir = glm::normalize(glm::vec3(randomX, 0.0f, randomZ));
        
        glm::vec3 smokePosition = glm::vec3(
            sourcePos.x + randomDir.x * radius,         // X casuale attorno alla sorgente
            sourcePos.y + (rand() % 30) / 100.0f,       // Y leggermente sopra la sorgente
            sourcePos.z + randomDir.z * radius          // Z casuale attorno alla sorgente
        );
        
        // Velocità prevalentemente verso l'alto con movimento laterale radiale per espansione
        float radialSpeed = 0.3f + (rand() % 40) / 100.0f; // Velocità di espansione radiale
        glm::vec3 smokeVelocity = glm::vec3(
            randomDir.x * radialSpeed * intensity,          // Movimento radiale X
            1.5f + (rand() % 100) / 200.0f * intensity,     // Velocità verso l'alto principale
            randomDir.z * radialSpeed * intensity           // Movimento radiale Z
        );
        
        // Aggiunge componente casuale per variazione naturale
        smokeVelocity.x += (rand() % 30 - 15) / 100.0f * intensity;
        smokeVelocity.z += (rand() % 30 - 15) / 100.0f * intensity;
        
        // Colore grigio con variazioni per realismo
        float smokeShade = 0.5f + (rand() % 30) / 100.0f; // 0.5-0.8 (dal grigio medio al grigio chiaro)
        float transparency = 0.3f + (rand() % 40) / 100.0f; // 0.3-0.7 (semi-trasparente, più denso)
        
        glm::vec4 smokeColor = glm::vec4(
            smokeShade,                             // R: grigio
            smokeShade,                             // G: grigio
            smokeShade,                             // B: grigio
            transparency                            // A: trasparenza variabile
        );
        
        // Dimensione iniziale piccola che crescerà nel tempo
        float smokeSize = 0.2f + (rand() % 15) / 100.0f;
        
        // Vita lunga per il fumo persistente
        float smokeLife = 6.0f + (rand() % 400) / 100.0f;
        
        spawnParticle(smokePosition, smokeVelocity, smokeColor, smokeSize, smokeLife);
        
        smokeTimer -= timePerPuff;
        puffsCreatedThisFrame++;
    }
}
