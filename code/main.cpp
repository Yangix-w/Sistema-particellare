#include <iostream>
#include <sstream>
#include "GL/glew.h" // prima di freeglut
#include "GL/freeglut.h"
#include "glm/glm.hpp"

#include "transform.h"
#include "camera.h"

#include "particleShader.h"
#include "texture2D.h"

#include "particleSystem.h"

GLint MODE = GL_FILL;

/**
  Struttura di comodo dove sono memorizzate tutte le variabili globali
*/
struct global_struct {

  int WINDOW_WIDTH  = 1024; // Larghezza della finestra 
  int WINDOW_HEIGHT = 768; // Altezza della finestra

  Camera camera;
  // Per il rendering delle particelle
  ParticleSystem particle_system;
  ParticleShader particle_shaders;
  
  // Posizione dell'emettitore di particelle
  glm::vec3 emitter_position;

  int renderType = 0; // Tipo di rendering da eseguire

  // fire texture
  bool fire_texture_loaded;
  // Flag per il fuoco
  bool trigger_fire;
  
  // exposion texture
  bool explosion_texture_loaded;
  // Flag per controllare l'esplosione
  bool trigger_explosion;
  
  // rain texture
  bool rain_texture_loaded;
  // Flag per controllare la pioggia
  bool trigger_rain;
  // Intensità della pioggia
  float rain_intensity;
  
  // smoke texture
  bool smoke_texture_loaded;
  // Flag per controllare il fumo
  bool trigger_smoke;
  // Intensità del fumo
  float smoke_intensity;

  global_struct() :  
                    emitter_position(0.0f, 0.0f, -10.0f), 
                    fire_texture_loaded(false), explosion_texture_loaded(false), rain_texture_loaded(false), smoke_texture_loaded(false),
                    trigger_fire(false), trigger_explosion(false), trigger_rain(false), trigger_smoke(false), 
                    rain_intensity(1.0f), smoke_intensity(1.0f) {}

} global;



/**
Prototipi della nostre funzioni di callback. 
Sono definite più avanti nel codice.
*/
void MyRenderScene(void);
void MyIdle(void);
void MyKeyboard(unsigned char key, int x, int y);
void MyClose(void);
void MySpecialKeyboard(int Key, int x, int y);
void MyMouse(int x, int y);

void init(int argc, char*argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
  
  
  glutInitWindowSize(global.WINDOW_WIDTH, global.WINDOW_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Sistema particellare");
  
  glutSetCursor(GLUT_CURSOR_NONE);
  
  global.camera.set_mouse_init_position(global.WINDOW_WIDTH/2, global.WINDOW_HEIGHT/2);
  global.camera.lock_mouse_position(true);
  glutWarpPointer(global.WINDOW_WIDTH/2, global.WINDOW_HEIGHT/2);
  
  // Must be done after glut is initialized!
  GLenum res = glewInit();
  if (res != GLEW_OK) {
    std::cerr<<"Error : "<<glewGetErrorString(res)<<std::endl;
    exit(1);
  }
  
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  
  glutDisplayFunc(MyRenderScene);
  
  glutKeyboardFunc(MyKeyboard);
  
  glutCloseFunc(MyClose);
  
  glutSpecialFunc(MySpecialKeyboard);
  
  glutPassiveMotionFunc(MyMouse);
  
  // Aggiungi funzione idle per rendering continuo
  glutIdleFunc(MyIdle);
  
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  
}

void MyIdle(void) {
  // Forza il re-rendering per animazioni continue
  glutPostRedisplay();
}

void create_scene() {
  global.camera.set_camera(
          glm::vec3(0, 0, 0),
          glm::vec3(0, 0,-1),
          glm::vec3(0, 1, 0)
      );

  global.camera.set_perspective(
    30.0f,
    global.WINDOW_WIDTH,
    global.WINDOW_HEIGHT,
    0.1,
    100
  );

  global.particle_shaders.init();
  global.particle_shaders.enable();
}

void renderParticles() {
  // Configurazioni OpenGL per particelle
  glDisable(GL_CULL_FACE); // Disabilita culling per vedere entrambi i lati delle particelle
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending
  glDepthMask(GL_FALSE); // Disabilita depth writing
  
  // Renderizza sistema di particelle con effetto fuoco
  if( global.trigger_fire) {
    if(global.explosion_texture_loaded){
      global.explosion_texture_loaded = false;
    }
    if(global.rain_texture_loaded){
      global.rain_texture_loaded = false;
    }
    if(global.smoke_texture_loaded){
      global.smoke_texture_loaded = false;
    }
    if(!global.fire_texture_loaded) {
      global.fire_texture_loaded = true;
      global.particle_system.loadTexture("texture/fire_texture.jpg");
    }
    global.renderType = 0; // Imposta il tipo di rendering per il fuoco
    global.particle_system.createFireSources(global.emitter_position, 1.4f);
  }

  
  // Esplosione, viene attivata una sola volta
  if (global.trigger_explosion) {
    if (global.fire_texture_loaded){
      global.fire_texture_loaded = false;
    }
    if (global.rain_texture_loaded){
      global.rain_texture_loaded = false;
    }
    if( global.smoke_texture_loaded){
      global.smoke_texture_loaded = false;
    }
    if (!global.explosion_texture_loaded){
      global.explosion_texture_loaded = true;
      global.particle_system.loadTexture("texture/explosion.png");
    }
    global.renderType = 1; // Imposta il tipo di rendering per l'esplosione
    global.particle_system.createExplosion(global.emitter_position, 1500, 10.0f);
    global.trigger_explosion = false; // Reset del flag
  }
  
  // Pioggia
  if (global.trigger_rain) {
    if (global.fire_texture_loaded) {
      global.fire_texture_loaded = false;
    }
    if (global.explosion_texture_loaded) {
      global.explosion_texture_loaded = false;
    }
    if (global.smoke_texture_loaded) {
      global.smoke_texture_loaded = false;
    }
    if (!global.rain_texture_loaded) {
      global.rain_texture_loaded = true;
      global.particle_system.loadTexture("texture/rain.gif");
    }
    global.renderType = 2; // Imposta il tipo di rendering per la pioggia
    global.particle_system.createRain(global.camera.position(), 10.0f, global.rain_intensity);
  }
  
  // Fumo
  if (global.trigger_smoke) {
    if (global.fire_texture_loaded) {
      global.fire_texture_loaded = false;
    }
    if (global.explosion_texture_loaded) {
      global.explosion_texture_loaded = false;
    }
    if (global.rain_texture_loaded) {
      global.rain_texture_loaded = false;
    }
    if (!global.smoke_texture_loaded) {
      global.smoke_texture_loaded = true;
      global.particle_system.loadTexture("texture/smoke.png");
    }
    global.renderType = 3; // Imposta il tipo di rendering per il fumo
    global.particle_system.createSmoke(global.emitter_position, global.smoke_intensity);
  }
  
  global.particle_system.render(global.renderType, global.camera.position());
  
  // Ripristina stato OpenGL
  glEnable(GL_CULL_FACE); // Riabilita culling per il resto della scena
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
}

void MyRenderScene() {
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  // Impostazioni per le particelle
  global.particle_shaders.set_camera_transform(global.camera.CP());
  renderParticles();

  glutSwapBuffers();
}

// Funzione globale che si occupa di gestire l'input da tastiera.
void MyKeyboard(unsigned char key, int x, int y) {
  switch ( key )
  {
    case 27: // Escape key
      glutDestroyWindow(glutGetWindow());
      return;
    break;

    case ' ': // Reimpostiamo la camera
      global.camera.set_camera(
          glm::vec3(0, 0, 0),
          glm::vec3(0, 0,-1),
          glm::vec3(0, 1, 0)
      );
    break;

    // Controlli per spostare l'emettitore di particelle
    case 'w': // Sposta emettitore più vicino
      global.emitter_position.z += 1.0f;
      std::cout << "Emettitore spostato a Z=" << global.emitter_position.z << std::endl;
    break;
    case 's': // Sposta emettitore più lontano
      global.emitter_position.z -= 1.0f;
      std::cout << "Emettitore spostato a Z=" << global.emitter_position.z << std::endl;
    break;
    case 'a': // Sposta emettitore a sinistra
      global.emitter_position.x -= 1.0f;
      std::cout << "Emettitore spostato a X=" << global.emitter_position.x << std::endl;
    break;
    case 'd': // Sposta emettitore a destra
      global.emitter_position.x += 1.0f;
      std::cout << "Emettitore spostato a X=" << global.emitter_position.x << std::endl;
    break;
    case 'z': // Sposta emettitore in basso
      global.emitter_position.y -= 1.0f;
      std::cout << "Emettitore spostato a Y=" << global.emitter_position.y << std::endl;
    break;
    case 'q': // Sposta emettitore in alto
      global.emitter_position.y += 1.0f;
      std::cout << "Emettitore spostato a Y=" << global.emitter_position.y << std::endl;
    break;

    case 'f': // Attiva/disattiva fuoco
      global.trigger_explosion = false; // Assicuriamoci che l'esplosione non sia attiva
      global.trigger_rain = false; // Assicuriamoci che la pioggia non sia attiva
      global.trigger_smoke = false; // Assicuriamoci che il fumo non sia attivo
      global.trigger_fire = !global.trigger_fire; // Inverte lo stato del fuoco
      std::cout << "Fuoco: " << (global.trigger_fire ? "ATTIVO" : "DISATTIVO") << std::endl;
      break;
    
    case 'e': // Attiva esplosione
      global.trigger_fire = false; // Assicuriamoci che il fuoco non sia attivo
      global.trigger_rain = false; // Assicuriamoci che la pioggia non sia attiva
      global.trigger_smoke = false; // Assicuriamoci che il fumo non sia attivo
      global.trigger_explosion = true; // Attiva l'esplosione
      std::cout << "Esplosione: ATTIVA" << std::endl;
    break;
    
    case 'r': // Attiva/disattiva pioggia
      global.trigger_fire = false; // Assicuriamoci che il fuoco non sia attivo
      global.trigger_explosion = false; // Assicuriamoci che l'esplosione non sia attiva
      global.trigger_smoke = false; // Assicuriamoci che il fumo non sia attivo
      global.trigger_rain = !global.trigger_rain; // Inverte lo stato della pioggia
      std::cout << "Pioggia: " << (global.trigger_rain ? "ATTIVA" : "DISATTIVA") << std::endl;
    break;
    
    case '1': // Diminuisce intensità pioggia (min 0.2)
      global.rain_intensity -= 0.2f;
      if (global.rain_intensity < 0.2f) global.rain_intensity = 0.2f;
      std::cout << "Intensità pioggia: " << global.rain_intensity << std::endl;
    break;
    
    case '2': // Aumenta intensità pioggia (max 3.0)
      global.rain_intensity += 0.2f;
      if (global.rain_intensity > 3.0f) global.rain_intensity = 3.0f;
      std::cout << "Intensità pioggia: " << global.rain_intensity << std::endl;
    break;

    case 'g': // Attiva/disattiva fumo
      global.trigger_fire = false; // Assicuriamoci che il fuoco non sia attivo
      global.trigger_explosion = false; // Assicuriamoci che l'esplosione non sia attiva
      global.trigger_rain = false; // Assicuriamoci che la pioggia non sia attiva
      global.trigger_smoke = !global.trigger_smoke; // Inverte lo stato del fumo
      std::cout << "Fumo: " << (global.trigger_smoke ? "ATTIVO" : "DISATTIVO") << std::endl;
    break;
    
    case '4': // Diminuisce intensità fumo (min 0.2)
      global.smoke_intensity -= 0.2f;
      if (global.smoke_intensity < 0.2f) global.smoke_intensity = 0.2f;
      std::cout << "Intensità fumo: " << global.smoke_intensity << std::endl;
    break;
    
    case '5': // Aumenta intensità fumo (max 3.0)
      global.smoke_intensity += 0.2f;
      if (global.smoke_intensity > 3.0f) global.smoke_intensity = 3.0f;
      std::cout << "Intensità fumo: " << global.smoke_intensity << std::endl;
    break;
  }

  glutPostRedisplay();
}

void MySpecialKeyboard(int Key, int x, int y) {
  global.camera.onSpecialKeyboard(Key);
  glutPostRedisplay();
}


void MyMouse(int x, int y) {
  if (global.camera.onMouse(x,y)) {
    // Risposto il mouse al centro della finestra
    glutWarpPointer(global.WINDOW_WIDTH/2, global.WINDOW_HEIGHT/2);
  }
  glutPostRedisplay();
}


// Funzione globale che si occupa di gestire la chiusura della finestra.
void MyClose(void) {
  std::cout << "Tearing down the system..." << std::endl;
  // Clean up here

  // A schermo intero dobbiamo uccidere l'applicazione.
  exit(0);
}

int main(int argc, char* argv[]){
  init(argc,argv);

  create_scene();

  glutMainLoop();
  
  return 0;
}