/* Neushoorn includes */
#include <nh_base.h>
#include <ext/nh_logging.h>

/* SDL includes */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>

/* stdlib includes */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Project headers */
#include "loadgl.h"

/* Structs */
typedef struct {
  nh_vec2_t pos;
  f32 min, max;
  const char *text;
  f32 *value;
  bool active;
  u8 for_active; /* Is SDL_SCANCODE */
  f32 sensitivity;
} slider_t;

/* Consts */
#define COMPUTE_WIDTH       512
#define COMPUTE_HEIGHT      512
#define FONT_COLS           24
#define FONT_ROWS           4

const char *font_chars =  " !\"#$%&'()*+,-./01234567"
                          "89:;<=>?@ABCDEFGHIJKLMNO"
                          "PQRSTUVWXYZ[\\]^_`abcdefg"
                          "hijklmnopqrstuvwxyz{|}~";
const f32 vertices[] = {
  -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
   1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
   1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
   1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
  -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
  -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
};

/* Globals */
struct {
  /* Window */
  SDL_Window *window;           /* Window handle */
  SDL_GLContext context;        /* OpenGL context handle */
  bool running;                 /* Is the window running? */
  i32 width, height;            /* Window dimensions */
  const u8 *keys;               /* Key states */
  /* OpenGL */
  u32 vbo;                      /* Vertex buffer object */
  u32 vao;                      /* Vertex array object */
  u32 shader_program;           /* Shader program */
  u32 texture;                  /* Texture */
  u32 font_texture;             /* Font texture */
  u32 compute_shader;           /* Compute shader */
  u32 solid_shader;             /* Solid shader */
  /* Other state */
  f32 focal_length;             /* Focal length */
  u32 ticks;                    /* Ticks since last movement */
  f32 delta_time;               /* Delta time */
  f32 fps;                      /* Frames per second */
  char fps_string[64];          /* FPS string */
  char delta_string[64];        /* Delta time string */
  f32 test_in;                  /* An input used for testing */
  f32 angle_x, angle_y;         /* Camera rotation: yaw, pitch */
  nh_vec3_t camera;             /* Camera position */
  u8 active_slider;             /* Active slider */
} state;

/* More state */
slider_t sliders[] = {
  (slider_t){(nh_vec2_t){-0.925f, 0.925f}, 0.0f, 10.0f, "[T] Test Input:   ", &state.test_in, false, SDL_SCANCODE_T, 20.0f},
  (slider_t){(nh_vec2_t){-0.925f, 0.875f}, 0.0f, 5.0f,  "[F] Focal Length: ", &state.focal_length, false, SDL_SCANCODE_F, 5.0f},
};

/* Utils */
char *get_file_contents(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NH_NULL) {
    return NH_NULL;
  }
  fseek(file, 0, SEEK_END);
  size_t filesize = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *contents = (char *)malloc(filesize + 1);
  fread(contents, 1, filesize, file);
  fclose(file);
  contents[filesize] = '\0';
  return contents;
}
void render_character(char c, nh_vec2_t pos, f32 scale) {
  /* RENDER CHARACTER */
  u32 char_vao, char_vbo;
  /* Create character vao */
  glGenVertexArrays(1, &char_vao);
  glBindVertexArray(char_vao);
  /* Find character */
  u8 char_row, char_col;
  bool char_found = false;
  for (u8 i = 0; i < FONT_ROWS * FONT_COLS; i++) {
    if (font_chars[i] == c) {
      char_row = i / FONT_COLS;
      char_col = i % FONT_COLS;
      char_found = true;
      break;
    }
  }
  if (!char_found) {
    char_row = 3;
    char_col = 23;
  }
  /* Create char_vertices */
  f32 x = 1.0f / (f32)FONT_COLS;
  f32 y = 1.0f / (f32)FONT_ROWS;
  f32 char_vertices[] = {
    pos.x-scale, pos.y-scale, 1.0f, x*char_col, y*(char_row+1),
    pos.x+scale, pos.y-scale, 1.0f, x*(char_col+1), y*(char_row+1),
    pos.x+scale, pos.y+scale, 1.0f, x*(char_col+1), y*char_row,
    pos.x-scale, pos.y-scale, 1.0f, x*char_col, y*(char_row+1),
    pos.x+scale, pos.y+scale, 1.0f, x*(char_col+1), y*char_row,
    pos.x-scale, pos.y+scale, 1.0f, x*char_col, y*char_row,
  };
  /* Create character vbo */
  glGenBuffers(1, &char_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, char_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * 6 * 5, char_vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)(3 * sizeof(f32)));
  /* Bind texture */
  glBindTexture(GL_TEXTURE_2D, state.font_texture);
  /* Render character */
  glUseProgram(state.shader_program);
  glBindVertexArray(char_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteVertexArrays(1, &char_vao);
  glDeleteBuffers(1, &char_vbo);
  glBindTexture(GL_TEXTURE_2D, 0);
}
f32 render_string(const char *str, nh_vec2_t pos, f32 scale) {
  f32 size = 0;
  for (u32 i = 0; str[i] != '\0'; i++) {
    render_character(str[i], (nh_vec2_t){pos.x+(i+1)*scale, pos.y}, scale);
    size += scale;
  }
  return size;
}
void render_slider(slider_t slider) {
  const f32 start_x = render_string(slider.text, slider.pos, 0.025f) + 0.025f;
  const f32 max_length = 0.5f;
  const f32 slider_x = start_x + max_length * (*(slider.value) - slider.min) / (slider.max - slider.min);
  /*render_string("=", (nh_vec2_t){slider.pos.x+slider_x, slider.pos.y}, 0.025f);*/
  render_string("]", (nh_vec2_t){slider.pos.x+start_x+max_length, slider.pos.y}, 0.025f);
  render_string("[", (nh_vec2_t){slider.pos.x+start_x-0.025f, slider.pos.y}, 0.025f);
  /* Create slider vao */
  u32 slider_vao, slider_vbo;
  glGenVertexArrays(1, &slider_vao);
  glBindVertexArray(slider_vao);
  /* Create slider_vertices - position, texture coords */
  f32 slider_vertices[] = {
    slider.pos.x + start_x,  slider.pos.y-0.025f,  0.5f,   0.0f, 0.0f,
    slider.pos.x + slider_x, slider.pos.y-0.025f,  0.5f,   1.0f, 0.0f,
    slider.pos.x + slider_x, slider.pos.y+0.025f,  0.5f,   1.0f, 1.0f,
    slider.pos.x + start_x,  slider.pos.y-0.025f,  0.5f,   0.0f, 0.0f,
    slider.pos.x + slider_x, slider.pos.y+0.025f,  0.5f,   1.0f, 1.0f,
    slider.pos.x + start_x,  slider.pos.y+0.025f,  0.5f,   0.0f, 1.0f
  };
  /* Create slider vbo */
  glGenBuffers(1, &slider_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, slider_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * 6 * 5, slider_vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)(3 * sizeof(f32)));
  /* Render slider */
  glUseProgram(state.solid_shader);
  if (slider.active) {
    nh_vec3_t color = (nh_vec3_t){1.0f, 1.0f, 1.0f};
    glUniform3fv(glGetUniformLocation(state.solid_shader, "color"), 1, (f32 *)&color);
  } else {
    nh_vec3_t color = (nh_vec3_t){0.4f, 0.4f, 0.4f};
    glUniform3fv(glGetUniformLocation(state.solid_shader, "color"), 1, (f32 *)&color);
  }
  glBindVertexArray(slider_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteVertexArrays(1, &slider_vao);
  glDeleteBuffers(1, &slider_vbo);
  glBindTexture(GL_TEXTURE_2D, 0);
}

/* Entry point */
int main(void) {
  /* Init SDL */
  NH_INFO("Initializing SDL...");
  NH_ASSERT_MSG(SDL_Init(SDL_INIT_VIDEO) == 0, "Failed to initialize SDL");
  IMG_Init(0);
  /* Create window */
  NH_INFO("Creating window...");
  state.window = SDL_CreateWindow(
    "OpenGL Compute Shader Path Tracing Demo",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    640,
    480,
    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
  );
  NH_ASSERT_MSG(state.window != NH_NULL, "Failed to create window");
  state.width = 640;
  state.height = 480;
  /* Enable OpenGL */
  NH_INFO("Enabling OpenGL...");
  SDL_GL_MakeCurrent(state.window, state.context);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_LoadLibrary(NULL);
  /* Create OpenGL context */
  NH_INFO("Creating OpenGL context...");
  state.context = SDL_GL_CreateContext(state.window);
  NH_ASSERT_MSG(state.context != NH_NULL, "Failed to create OpenGL context");
  NH_LOG_ENTRY("Version: %s", glGetString(GL_VERSION));
  NH_LOG_ENTRY("Vendor: %s", glGetString(GL_VENDOR));
  NH_LOG_ENTRY("Renderer: %s", glGetString(GL_RENDERER));

  /* Create viewport */
  NH_INFO("Creating viewport...");
  glViewport(0, 0, state.width, state.height);

  /* Set clear color */
  NH_INFO("Setting clear color...");
  NH_LOG_ENTRY("Clear color set to %f, %f, %f, %f", 0.0f, 1.0f, 0.8f, 1.0f);
  /*glClearColor(0.0f, 1.0f, 0.8f, 1.0f);*/
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Load OpenGL functions */
  NH_INFO("Loading OpenGL functions...");
  NH_ASSERT_MSG(loadGL(), "Failed to load OpenGL functions");

  /* Create vertex array object */
  NH_INFO("Creating vertex array object...");
  glGenVertexArrays(1, &state.vao);
  glBindVertexArray(state.vao);

  /* Create vertex buffer object */
  NH_INFO("Creating vertex buffer object...");
  glGenBuffers(1, &state.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, state.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  /* Link vertex attributes */
  NH_INFO("Linking vertex attributes...");
  /* Position */
  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)0);
  glEnableVertexAttribArray(0);
  /* Texture coordinates */
  glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)(3 * sizeof(f32)));
  glEnableVertexAttribArray(1);

  /* Unbind vertex array object */
  glBindVertexArray(0);

  /* Create shader program */
  NH_INFO("Creating shader program...");
  int success;
  char info_log[512];
  /* Create vertex shader */
  NH_LOG_ENTRY("Creating vertex shader...");
  u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  /* Read from shader.vertex */
  char *vert_shader_source = get_file_contents("shader.vertex");
  glShaderSource(vertex_shader, 1, (const char**)&vert_shader_source, NULL);
  free(vert_shader_source);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    NH_ERROR("Failed to compile vertex shader: %s", info_log);
  }
  /* Create fragment shader */
  NH_LOG_ENTRY("Creating fragment shader...");
  u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  /* Read from shader.fragment */
  char *frag_shader_source = get_file_contents("shader.fragment");
  glShaderSource(fragment_shader, 1, (const char**)&frag_shader_source, NULL);
  free(frag_shader_source);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    NH_ERROR("Failed to compile fragment shader: %s", info_log);
  }

  /* Create shader program */
  NH_LOG_ENTRY("Creating shader program...");
  state.shader_program = glCreateProgram();
  glAttachShader(state.shader_program, vertex_shader);
  glAttachShader(state.shader_program, fragment_shader);
  glLinkProgram(state.shader_program);
  glGetProgramiv(state.shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(state.shader_program, 512, NULL, info_log);
    NH_ERROR("Failed to link shader program: %s", info_log);
  }
  /* Cleanup */
  NH_LOG_ENTRY("Cleaning up...");
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  glUseProgram(0);

  /* Create compute shader */
  NH_INFO("Creating compute shader...");
  NH_LOG_ENTRY("Creating compute shader...");
  u32 compute_shader = glCreateShader(GL_COMPUTE_SHADER);

  /* Read from shader.compute */
  char *compute_shader_source = get_file_contents("shader.compute");
  glShaderSource(compute_shader, 1, (const char**)&compute_shader_source, NULL);
  free(compute_shader_source);

  glCompileShader(compute_shader);
  glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(compute_shader, 512, NULL, info_log);
    NH_ERROR("Failed to compile compute shader: %s", info_log);
  }
  /* Create compute program */
  NH_LOG_ENTRY("Creating compute program...");
  state.compute_shader = glCreateProgram();
  glAttachShader(state.compute_shader, compute_shader);
  glLinkProgram(state.compute_shader);
  glGetProgramiv(state.compute_shader, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(state.compute_shader, 512, NULL, info_log);
    NH_ERROR("Failed to link compute program: %s", info_log);
  }
  /* Cleanup */
  NH_LOG_ENTRY("Cleaning up...");
  glDeleteShader(state.compute_shader);
  glUseProgram(0);

  /* Create shader program */
  NH_INFO("Creating solid shader program...");
  /* Create vertex shader */
  NH_LOG_ENTRY("Creating vertex shader...");
  u32 solid_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  /* Read from shader.vertex */
  char *solid_vert_shader_source = get_file_contents("shader.vertex");
  glShaderSource(solid_vertex_shader, 1, (const char**)&solid_vert_shader_source, NULL);
  free(solid_vert_shader_source);
  glCompileShader(solid_vertex_shader);
  glGetShaderiv(solid_vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(solid_vertex_shader, 512, NULL, info_log);
    NH_ERROR("Failed to compile vertex shader: %s", info_log);
  }
  /* Create fragment shader */
  NH_LOG_ENTRY("Creating fragment shader...");
  u32 solid_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  /* Read from solid.fragment */
  char *solid_frag_shader_source = get_file_contents("solid.fragment");
  glShaderSource(solid_fragment_shader, 1, (const char**)&solid_frag_shader_source, NULL);
  free(solid_frag_shader_source);
  glCompileShader(solid_fragment_shader);
  glGetShaderiv(solid_fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(solid_fragment_shader, 512, NULL, info_log);
    NH_ERROR("Failed to compile fragment shader: %s", info_log);
  }

  /* Create shader program */
  NH_LOG_ENTRY("Creating shader program...");
  state.solid_shader = glCreateProgram();
  glAttachShader(state.solid_shader, solid_vertex_shader);
  glAttachShader(state.solid_shader, solid_fragment_shader);
  glLinkProgram(state.solid_shader);
  glGetProgramiv(state.solid_shader, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(state.solid_shader, 512, NULL, info_log);
    NH_ERROR("Failed to link shader program: %s", info_log);
  }
  /* Cleanup */
  NH_LOG_ENTRY("Cleaning up...");
  glDeleteShader(solid_vertex_shader);
  glDeleteShader(solid_fragment_shader);
  glUseProgram(0);


  /* Create texture */
  NH_INFO("Creating main texture...");
  glGenTextures(1, &state.texture);
  glBindTexture(GL_TEXTURE_2D, state.texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/
  /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA32F,
      COMPUTE_WIDTH, /* Width */
      COMPUTE_HEIGHT, /* Height */
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      NULL
  );
  glBindImageTexture(0, state.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
  glBindTexture(GL_TEXTURE_2D, 0);
  /* Create font texture */
  NH_INFO("Creating font texture...");
  glGenTextures(1, &state.font_texture);
  glBindTexture(GL_TEXTURE_2D, state.font_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  SDL_Surface *font_surface = IMG_Load("font.png");
  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      font_surface->w,
      font_surface->h,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      font_surface->pixels
  );
  SDL_FreeSurface(font_surface);
  glBindTexture(GL_TEXTURE_2D, 0);

  /* Main loop */
  state.running = true;
  state.focal_length = 1.0f;
  state.test_in = 0.0f;
  state.angle_x = 0.0f;
  state.angle_y = -0.3f;
  state.camera = (nh_vec3_t){0.0f, 1.5f, 0.0f};
  state.keys = SDL_GetKeyboardState(NULL);
  state.active_slider = 0;
  sliders[0].active = true;
  while (state.running) {
    /* Delta time - 1 */
    u64 start = SDL_GetPerformanceCounter();

    /* Event loop */
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case (SDL_QUIT): {
          state.running = false;
        } break;
        case (SDL_WINDOWEVENT): {
          if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            /* Log window size */
            NH_INFO(
                "Window resized to %dx%d",
                event.window.data1,
                event.window.data2
            );
            /* Update viewport */
            glViewport(0, 0, event.window.data1, event.window.data2);
            /* Update state */
            state.width = event.window.data1;
            state.height = event.window.data2;
            /* Reset ticks */
            state.ticks = 0;
          }
        } break;
      }
    }

    /* Check for keydown */
    /* I = increase focal length (zoom in) */
    if (state.keys[SDL_SCANCODE_I]) {
      state.focal_length += 2.0f * state.delta_time;
      state.ticks = 0;
    }
    /* O = decrease focal length (zoom out) */
    if (state.keys[SDL_SCANCODE_O]) {
      state.focal_length -= 2.0f * state.delta_time;
      state.ticks = 0;
    }

    /* Right = rotate right */
    if (state.keys[SDL_SCANCODE_RIGHT]) {
      state.angle_x -= 1.0f * state.delta_time;
      state.ticks = 0;
    }
    /* Left = rotate left */
    if (state.keys[SDL_SCANCODE_LEFT]) {
      state.angle_x += 1.0f * state.delta_time;
      state.ticks = 0;
    }
    /* Up = rotate up */
    if (state.keys[SDL_SCANCODE_UP]) {
      state.angle_y += 1.0f * state.delta_time;
      state.ticks = 0;
    }
    /* Down = rotate down */
    if (state.keys[SDL_SCANCODE_DOWN]) {
      state.angle_y -= 1.0f * state.delta_time;
      state.ticks = 0;
    }

    /* W = move forward */
    if (state.keys[SDL_SCANCODE_W]) {
      state.camera.z += 5.0f * state.delta_time;
      state.ticks = 0;
    }
    /* S = move backward */
    if (state.keys[SDL_SCANCODE_S]) {
      state.camera.z -= 5.0f * state.delta_time;
      state.ticks = 0;
    }
    /* A = move left */
    if (state.keys[SDL_SCANCODE_A]) {
      state.camera.x -= 5.0f * state.delta_time;
      state.ticks = 0;
    }
    /* D = move right */
    if (state.keys[SDL_SCANCODE_D]) {
      state.camera.x += 5.0f * state.delta_time;
      state.ticks = 0;
    }
    /* Space = move up */
    if (state.keys[SDL_SCANCODE_SPACE]) {
      state.camera.y += 5.0f * state.delta_time;
      state.ticks = 0;
    }
    /* Shift = move down */
    if (state.keys[SDL_SCANCODE_LSHIFT]) {
      state.camera.y -= 5.0f * state.delta_time;
      state.ticks = 0;
    }

#if 0
    /* Plus = increase test in */
    if (state.keys[SDL_SCANCODE_KP_PLUS]) {
      state.test_in += 20.0f * state.delta_time;
      if (state.test_in > 5.0f) state.test_in = 5.0f;
      state.ticks = 0;
    }
    /* Minus = decrease test in */
    if (state.keys[SDL_SCANCODE_KP_MINUS]) {
      state.test_in -= 20.0f * state.delta_time;
      if (state.test_in < 0.0f) state.test_in = 0.0f;
      state.ticks = 0;
    }
#endif
    /* Plus = increase active slider */
    if (state.keys[SDL_SCANCODE_KP_PLUS]) {
      *(sliders[state.active_slider].value) += sliders[state.active_slider].sensitivity * state.delta_time;
      if (*(sliders[state.active_slider].value) > sliders[state.active_slider].max) {
        *(sliders[state.active_slider].value) = sliders[state.active_slider].max;
      }
      state.ticks = 0;
    }
    /* Minus = decrease active slider */
    if (state.keys[SDL_SCANCODE_KP_MINUS]) {
      *(sliders[state.active_slider].value) -= sliders[state.active_slider].sensitivity * state.delta_time;
      if (*(sliders[state.active_slider].value) < sliders[state.active_slider].min) {
        *(sliders[state.active_slider].value) = sliders[state.active_slider].min;
      }
      state.ticks = 0;
    }

    /* M+W = wireframe mode */
    if (state.keys[SDL_SCANCODE_M]
        && state.keys[SDL_SCANCODE_W]) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    /* M+R = normal mode */
    if (state.keys[SDL_SCANCODE_M]
        && state.keys[SDL_SCANCODE_R]) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    /* Sliders */
    for (u32 i = 0; i < sizeof(sliders) / sizeof(sliders[0]); i++) {
      if (state.keys[sliders[i].for_active]) {
        sliders[state.active_slider].active = false;
        state.active_slider = i;
        sliders[i].active = true;
      }
    }

    /* Clear screen */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Render */
    glUseProgram(state.shader_program);
    glBindVertexArray(state.vao);

    /* Bind texture */
    glBindTexture(GL_TEXTURE_2D, state.texture);
    glActiveTexture(GL_TEXTURE0);

    /* Set uniforms */
    glUseProgram(state.compute_shader);
    glUniform1f(glGetUniformLocation(state.compute_shader, "width"), (f32)state.width);
    glUniform1f(glGetUniformLocation(state.compute_shader, "height"), (f32)state.height);
    glUniform1f(glGetUniformLocation(state.compute_shader, "focal_length"), state.focal_length);
    glUniform1f(glGetUniformLocation(state.compute_shader, "angle_x"), state.angle_x);
    glUniform1f(glGetUniformLocation(state.compute_shader, "angle_y"), state.angle_y);
    glUniform1f(glGetUniformLocation(state.compute_shader, "test_in"), (f32)state.test_in);
    glUniform1ui(glGetUniformLocation(state.compute_shader, "random_seed"), rand());
    glUniform1ui(glGetUniformLocation(state.compute_shader, "ticks"), state.ticks);
    glUniform3fv(glGetUniformLocation(state.compute_shader, "camera"), 1, (f32 *)&state.camera);

    /* Dispatch compute shader */
    glDispatchCompute(COMPUTE_WIDTH / 32, COMPUTE_HEIGHT / 32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    /* Draw */
    glUseProgram(state.shader_program);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Unbind */
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    /* Render hints */
    render_string("[M]+[W]      = wireframe", (nh_vec2_t){-0.925f, -0.925f}, 0.025f);
    render_string("[M]+[R]      = regular", (nh_vec2_t){-0.925f, -0.875f}, 0.025f);
    render_string("[W][A][S][D] = move", (nh_vec2_t){-0.925f, -0.825f}, 0.025f);
    render_string("<ARROW-KEYS> = look", (nh_vec2_t){-0.925f, -0.775f}, 0.025f);
    /* Render sliders */
    for (u32 i = 0; i < sizeof(sliders) / sizeof(sliders[0]); i++) {
      render_slider(sliders[i]);
    }
    /* Draw delta time and FPS to the screen */
    render_string(state.delta_string, (nh_vec2_t){0.5f, 0.925f}, 0.025f);
    render_string(state.fps_string, (nh_vec2_t){0.5f, 0.875f}, 0.025f);

    /* Swap buffers */
    SDL_GL_SwapWindow(state.window);

    /* Increment ticks */
    state.ticks++;

    /* Delta time - 2 */
    u64 end = SDL_GetPerformanceCounter();
    f64 delta = (f64)(end - start) / (f64)SDL_GetPerformanceFrequency();
    state.delta_time = delta;
    state.fps = 1.0f / delta;
    if (state.ticks % 60 == 0) {
      /*NH_INFO("Delta time: %.2fms, FPS: %f", delta * 1000.0, state.fps);*/
      sprintf(state.fps_string, "FPS: %f", state.fps);
      sprintf(state.delta_string, "Delta time: %.2fms", delta * 1000.0);
    }
  }

  /* Clean up */
  NH_INFO("Cleaning up...");
  glDeleteTextures(1, &state.font_texture);
  glDeleteTextures(1, &state.texture);
  glDeleteProgram(state.compute_shader);
  glDeleteProgram(state.shader_program);
  glDeleteBuffers(1, &state.vbo);
  glDeleteVertexArrays(1, &state.vao);
  SDL_GL_DeleteContext(state.context);
  SDL_DestroyWindow(state.window);
  IMG_Quit();
  SDL_Quit();
  return 0;
}
