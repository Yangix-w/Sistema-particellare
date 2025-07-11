#version 330

// Input dal vertex shader
in vec4 fragment_color;
in vec2 texture_coords;

// Texture sampler
uniform sampler2D particle_texture;

// Output del fragment shader
out vec4 out_color;

void main(){
    // Campiona la texture
    vec4 tex_color = texture(particle_texture, texture_coords);
    
    // Combina il colore della texture con il colore della particella
    vec4 final_color = fragment_color * tex_color;
    
    out_color = final_color;
}
