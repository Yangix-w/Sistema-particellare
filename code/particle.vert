#version 330

// Attributi per ogni vertice del billboard (quad)
layout (location = 0) in vec3 vertex_position; // Posizioni relative del quad (-0.5 a 0.5)

// Attributi per ogni istanza di particella (instanced attributes)
layout (location = 1) in vec4 particle_center_size; // xyz = centro particella, w = dimensione
layout (location = 2) in vec4 particle_color;       // rgba = colore particella

// Matrici di trasformazione
uniform mat4 World2Camera;

// Output verso fragment shader
out vec4 fragment_color;
out vec2 texture_coords;

void main(){

    vec3 center = particle_center_size.xyz;
    float size = particle_center_size.w;
    
    // Calcola le coordinate texture dal vertex_position
    // vertex_position va da -0.5 a 0.5, quindi le mappiamo a 0.0-1.0
    texture_coords = vertex_position.xy + 0.5;
    
    // Calcola la posizione finale del vertice
    vec3 vertex_world = center + vertex_position * size;
    
    // Applica la trasformazione completa
    gl_Position = World2Camera * vec4(vertex_world, 1.0);
    
    // Passa il colore al fragment shader
    fragment_color = particle_color;
}
