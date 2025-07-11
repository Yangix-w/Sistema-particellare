#include "particleShader.h"
#include "utilities.h"
#include "common.h"

ParticleShader::ParticleShader() : _camera_transform_location(-1), _texture_location(-1) {
}

bool ParticleShader::load_shaders() {
    return add_shader(GL_VERTEX_SHADER, "particle.vert") &&
           add_shader(GL_FRAGMENT_SHADER, "particle.frag");
}

bool ParticleShader::load_done() {
    // Ottieni le location degli uniform
    _camera_transform_location = get_uniform_location("World2Camera");
    _texture_location = get_uniform_location("particle_texture");
    
    // Imposta la texture unit di default
    if (_texture_location != INVALID_UNIFORM_LOCATION) {
        enable();
        glUniform1i(_texture_location, 0); // Texture unit 0
    }
    
    return _camera_transform_location != INVALID_UNIFORM_LOCATION &&
           _texture_location != INVALID_UNIFORM_LOCATION;
}

void ParticleShader::set_camera_transform(const glm::mat4 &transform) {
    glUniformMatrix4fv(_camera_transform_location, 1, GL_FALSE, const_cast<float *>(&transform[0][0]));
}
