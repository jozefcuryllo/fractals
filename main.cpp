#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <array>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "fractals/factory.h"

struct App {
  SDL_Window *win = nullptr;
  SDL_Renderer *ren = nullptr;
  TTF_Font *font_small = nullptr;
  TTF_Font *font_big = nullptr;

  std::unique_ptr<Fractal> fractal;
  FractalType fractal_type = FractalType::MANDELBROT;

  float speed = 1.0f;
  float fps = 0.0f;
  int win_w = 1280;
  int win_h = 720;
  int fractal_h = 0;

  bool running = true;
  bool resize_pending = false;
  bool show_help = true;
  bool paused = false;

  Uint32 last_ticks = 0;
  int frame_counter = 0;
  Uint32 fps_timer = 0;
};

TTF_Font *try_load_font(const char *path, int size) {
  return TTF_OpenFont(path, size);
}

bool setup(App &app) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    return false;
  }

  if (TTF_Init() != 0) {
    SDL_Quit();
    return false;
  }

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  app.win_w = dm.w * 3 / 4;
  app.win_h = dm.h * 3 / 4;
  app.fractal_h = app.win_h - 40;

  app.win = SDL_CreateWindow("Fractals", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, app.win_w, app.win_h,
                             SDL_WINDOW_RESIZABLE);

  if (!app.win) {
    TTF_Quit();
    SDL_Quit();
    return false;
  }

  app.ren = SDL_CreateRenderer(
      app.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (!app.ren) {
    SDL_DestroyWindow(app.win);
    TTF_Quit();
    SDL_Quit();
    return false;
  }

  std::vector<std::string> font_paths = {
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", "DejaVuSans.ttf"};

  for (const auto &path : font_paths) {
    app.font_small = try_load_font(path.c_str(), 14);
    if (app.font_small) {
      app.font_big = try_load_font(path.c_str(), 18);
      if (app.font_big)
        break;
    }
  }

  if (!app.font_small) {
    app.font_small = TTF_OpenFont(nullptr, 14);
    app.font_big = TTF_OpenFont(nullptr, 18);
  }

  app.fractal = createFractal(app.fractal_type, app.ren);
  if (app.fractal) {
    app.fractal->resize(app.win_w, app.fractal_h);
  }

  app.last_ticks = SDL_GetTicks();
  app.fps_timer = app.last_ticks;

  return true;
}

void draw_text(SDL_Renderer *ren, TTF_Font *font, int x, int y,
               const char *text, SDL_Color color) {
  SDL_Surface *surf = TTF_RenderUTF8_Blended(font, text, color);
  if (!surf)
    return;

  SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
  SDL_Rect dst = {x, y, surf->w, surf->h};
  SDL_RenderCopy(ren, tex, nullptr, &dst);

  SDL_DestroyTexture(tex);
  SDL_FreeSurface(surf);
}

void draw_menu(App &app) {
  SDL_Rect bar = {0, app.fractal_h, app.win_w, 40};
  SDL_SetRenderDrawColor(app.ren, 30, 32, 40, 255);
  SDL_RenderFillRect(app.ren, &bar);

  SDL_SetRenderDrawColor(app.ren, 80, 85, 100, 255);
  SDL_RenderDrawLine(app.ren, 0, bar.y, app.win_w, bar.y);

  char buf[256];
  const char *name = getFractalName(app.fractal_type);
  snprintf(buf, sizeof(buf), "%s | Speed: %.1fx | FPS: %.1f | %s", name,
           app.speed, app.fps, app.paused ? "[PAUSED]" : "");

  SDL_Color col = {220, 230, 255, 255};
  draw_text(app.ren, app.font_small, 10, bar.y + 12, buf, col);
}

void draw_help(App &app) {
  if (!app.show_help)
    return;

  SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 180);
  SDL_RenderFillRect(app.ren, nullptr);

  int w = 400;
  int h = 280;
  int x = (app.win_w - w) / 2;
  int y = (app.win_h - h) / 2;

  SDL_Rect box = {x, y, w, h};
  SDL_SetRenderDrawColor(app.ren, 40, 42, 50, 255);
  SDL_RenderFillRect(app.ren, &box);
  SDL_SetRenderDrawColor(app.ren, 100, 150, 220, 255);
  SDL_RenderDrawRect(app.ren, &box);

  SDL_Color white = {255, 255, 255, 255};
  SDL_Color blue = {150, 200, 255, 255};

  draw_text(app.ren, app.font_big, x + 20, y + 20, "Controls", blue);

  std::array<const char *, 9> lines = {"1-9  - Change fractal",
                                       "+/-  - Change speed",
                                       "SPACE- Pause animation",
                                       "F    - Toggle fullscreen",
                                       "H    - Show/hide help",
                                       "ESC  - Quit",
                                       "",
                                       "Click or press any key",
                                       "to start..."};

  for (size_t i = 0; i < lines.size(); i++) {
    draw_text(app.ren, app.font_small, x + 30, y + 60 + (int)i * 22, lines[i],
              white);
  }

  SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_NONE);
}

void process_events(App &app) {
  SDL_Event ev;
  while (SDL_PollEvent(&ev)) {
    if (ev.type == SDL_QUIT) {
      app.running = false;
      return;
    }

    if (app.show_help &&
        (ev.type == SDL_KEYDOWN || ev.type == SDL_MOUSEBUTTONDOWN)) {
      app.show_help = false;
      continue;
    }

    if (ev.type == SDL_WINDOWEVENT &&
        ev.window.event == SDL_WINDOWEVENT_RESIZED) {
      app.win_w = ev.window.data1;
      app.win_h = ev.window.data2;
      app.fractal_h = app.win_h - 40;
      app.resize_pending = true;
    }

    if (ev.type == SDL_KEYDOWN) {
      bool full;

      switch (ev.key.keysym.sym) {
      case SDLK_ESCAPE:
        app.running = false;
        break;

      case SDLK_h:
        app.show_help = !app.show_help;
        break;

      case SDLK_SPACE:
        app.paused = !app.paused;
        break;

      case SDLK_PLUS:
      case SDLK_EQUALS:
      case SDLK_KP_PLUS:
        app.speed += 0.1f;
        if (app.speed > 10.0f)
          app.speed = 10.0f;
        break;

      case SDLK_MINUS:
      case SDLK_KP_MINUS:
        app.speed -= 0.1f;
        if (app.speed < 0.0f)
          app.speed = 0.0f;
        break;

      case SDLK_f:
        full = (SDL_GetWindowFlags(app.win) & SDL_WINDOW_FULLSCREEN) != 0;
        SDL_SetWindowFullscreen(app.win,
                                full ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
        break;

      default:
        if (ev.key.keysym.sym >= SDLK_1 && ev.key.keysym.sym <= SDLK_9) {
          int idx = ev.key.keysym.sym - SDLK_1;
          if (idx < (int)FractalType::COUNT) {
            app.fractal_type = (FractalType)idx;
            app.fractal = createFractal(app.fractal_type, app.ren);
            if (app.fractal) {
              app.fractal->resize(app.win_w, app.fractal_h);
            }
          }
        }
        break;
      }
    }
  }
}

void update_fps(App &app) {
  app.frame_counter++;

  Uint32 now = SDL_GetTicks();
  Uint32 elapsed = now - app.fps_timer;

  if (elapsed >= 1000) {
    app.fps = app.frame_counter * 1000.0f / elapsed;
    app.frame_counter = 0;
    app.fps_timer = now;
  }
}

void run(App &app) {
  while (app.running) {
    Uint32 now = SDL_GetTicks();
    float dt = (now - app.last_ticks) / 1000.0f;
    app.last_ticks = now;

    if (dt > 0.1f)
      dt = 0.1f;

    process_events(app);

    if (app.resize_pending && app.fractal) {
      app.fractal->resize(app.win_w, app.fractal_h);
      app.resize_pending = false;
    }

    if (app.fractal && !app.paused) {
      app.fractal->update(dt * app.speed, 16);
    }

    SDL_SetRenderDrawColor(app.ren, 18, 20, 25, 255);
    SDL_RenderClear(app.ren);

    if (app.fractal) {
      app.fractal->render();
    }

    draw_menu(app);
    draw_help(app);

    SDL_RenderPresent(app.ren);
    update_fps(app);
  }
}

void cleanup(App &app) {
  if (app.fractal) {
    app.fractal.reset();
  }

  if (app.font_big) {
    TTF_CloseFont(app.font_big);
  }

  if (app.font_small) {
    TTF_CloseFont(app.font_small);
  }

  if (app.ren) {
    SDL_DestroyRenderer(app.ren);
  }

  if (app.win) {
    SDL_DestroyWindow(app.win);
  }

  TTF_Quit();
  SDL_Quit();
}

int main(int argc, char **argv) {
  App app;

  if (!setup(app)) {
    return 1;
  }

  run(app);
  cleanup(app);

  return 0;
}