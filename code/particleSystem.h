#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "particle.h"
#include "texture2D.h"

const int MAX_PARTICLES = 5000; // Numero massimo di particelle

/**
    Classe per gestire un sistema di particelle.
    Gestisce la creazione, il rendering e l'ordinamento delle particelle.
*/
class ParticleSystem {
public:
    GLuint _VAO;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;
    Texture2D particle_texture;         // Texture per le particelle

    Particle *particles[MAX_PARTICLES]; // Array di particelle
    int lastUsed = 0;                   // Indice dell'ultima particella utilizzata
    bool arrayInit = false;             // Flag per verificare se l'array è stato inizializzato
    bool initialized = false;           // Flag per verificare se il sistema è stato inizializzato

    // Variabili per il rendering delle particelle
    GLfloat g_particule_position_size_data[MAX_PARTICLES * 4]; // Array con posizioni e dimensioni
    GLubyte g_particule_color_data[MAX_PARTICLES * 4];         // Array con i colori delle particelle

    /**
        Costruttore di default per inizializzare il sistema di particelle.
    */
    ParticleSystem();

    /**
        Distruttore per deallocare le risorse del sistema di particelle.
        Dealloca le particelle e rilascia le risorse OpenGL.
    */
    ~ParticleSystem();

    /**
        Costruttore di copia per creare una nuova istanza del sistema di particelle come copia di un'altra.
    */
    ParticleSystem(const ParticleSystem &other);

    /**
        Operatore di assegnazione per copiare i valori da un altro sistema di particelle.
    */
    ParticleSystem &operator=(const ParticleSystem &other);

    /**
        Inizializza il sistema di particelle.
        Crea e configura i buffer OpenGL per le particelle.
        Inizializza le particelle con valori predefiniti.
    */
    void init();

    /**
        Carica una texture per le particelle.
        @param texturePath Il percorso del file della texture.
        @return true se la texture è stata caricata correttamente, false altrimenti.
    */
    bool loadTexture(const std::string& texturePath);

    /**
        Logica di rendering per le particelle.
        @param cameraPosition La posizione della camera per calcolare la distanza delle particelle.
    */
    void fireRender(const glm::vec3 &cameraPosition);       // Renderizza le particelle per l'effetto fuoco
    void explosionRender(const glm::vec3 &cameraPosition);  // Renderizza le particelle per l'effetto esplosione
    void rainRender(const glm::vec3 &cameraPosition);       // Renderizza le particelle per l'effetto pioggia
    void smokeRender(const glm::vec3 &cameraPosition);      // Renderizza le particelle per l'effetto fumo

    /**
        Renderizza le particelle in base al tipo specificato.
        @param renderType Il tipo di rendering da eseguire (0: Fuoco, 1: Esplosione, 2: Pioggia, 3: Fumo).
        @param cameraPosition La posizione della camera per calcolare la distanza delle particelle.
    */
    void render(const int renderType, const glm::vec3 &cameraPosition);

    /**
        Ordina le particelle in base alla loro distanza dalla camera.ù
    */
    void sortParticles();

    /**
        Elimina una particella specificata dall'indice.
        @param index L'indice della particella da eliminare.
        Se l'indice è fuori dai limiti, non fa nulla.
        Imposta la particella come "morta" (vita = 0).
    */
    void killParticle(int index);

    /**
        Trova una particella inutilizzata (con vita <= 0).
        Scansiona l'array di particelle per trovare la prima particella morta.
        Se non trova nessuna particella morta, ritorna -1.
        @return L'indice della prima particella inutilizzata, o -1 se tutte le particelle sono in uso.
    */
    int findUnusedParticle();

    /**
        Crea una nuova particella con le specifiche fornite.
        Se non ci sono particelle inutilizzate, forza la morte della più vecchia.
    */
    void spawnParticle(glm::vec3 pos, glm::vec3 vel, glm::vec4 col,
                       float particleSize, float particleLife);

    /**
        Emettitori di particelle per vari effetti.
    */
    void createFireSources(glm::vec3 basePos, float intensity = 1.0f);          // Fuoco
    void createExplosion(glm::vec3 center, int numParticles, float force);      // Esplosione
    void createRain(glm::vec3 areaCenter, float areaSize, float intensity);     // Effetto pioggia
    void createSmoke(glm::vec3 sourcePos, float intensity = 1.0f);              // Effetto fumo
}; 

#endif // PARTICLE_SYSTEM_H