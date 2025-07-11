#ifndef PARTICLE_H
#define PARTICLE_H

#include "glm/glm.hpp"
#include "GL/glew.h"

/**
    Classe per rappresentare una particella nel sistema di particelle.
*/
class Particle {
    public:
        glm::vec3 position;     // Posizione della particella
        glm::vec3 velocity;     // Velocità della particella
        glm::vec4 color;        // Colore della particella
        float size;             // Dimensione della particella
        float life;             // Vita residua della particella
        float cameradistance;   // Distanza dalla camera per l'ordinamento
        
        /**
            Costruttore di default per inizializzare una particella con valori predefiniti.
         */
        Particle();

        /**
            Costruttore parametrico per inizializzare una particella con valori specifici.
         */
        Particle(glm::vec3 position, glm::vec3 velocity, glm::vec4 color,
                float size, float lifetime);

        /**
            Distruttore della particella.
         */
        ~Particle();

        /**
            Costruttore di copia per creare una nuova particella come copia di un'altra.
         */
        Particle(const Particle &other);

        /**
            Operatore di assegnazione per copiare i valori da un'altra particella.
         */
        Particle &operator=(const Particle &other);

        /**
            Operatore di confronto per ordinare le particelle in base alla distanza dalla camera.
         */
        bool operator<(Particle &other);
};

#endif // PARTICLE_H