#ifndef PARTICLE_SHADER_H
#define PARTICLE_SHADER_H

#include "shaderclass.h"
#include "glm/glm.hpp"

/**
    Classe shader specifica per il rendering delle particelle
*/
class ParticleShader : public ShaderClass {
private:
    GLint _camera_transform_location;
    GLint _texture_location;

public:
    ParticleShader();
    
    /**
        Carica gli shader specifici per particelle
    */
    bool load_shaders() override;
    
    /**
        Inizializza le location degli uniform dopo il linking
    */
    bool load_done() override;
    
    /**
        Setta la matrice di trasformazione di camera
        @param transform matrice 4x4 di trasformazione  
    */
    void set_camera_transform(const glm::mat4 &transform);
};

#endif // PARTICLE_SHADER_H
