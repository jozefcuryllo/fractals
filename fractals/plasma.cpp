#include "fractal.h"
#include <cmath>
#include <deque>
#include <vector>

class Plasma : public FractalFB {
public:
  Plasma(SDL_Renderer *r) : FractalFB(r) {}

  struct RectArea {
    int x1, y1, x2, y2;
    float roughness;
  };

  void reset() override {
    clear();
    pendingAreas.clear();

    grid.assign((width + 1) * (height + 1), 0.0f);
    setGrid(0, 0, randFloat());
    setGrid(width - 1, 0, randFloat());
    setGrid(0, height - 1, randFloat());
    setGrid(width - 1, height - 1, randFloat());

    pendingAreas.push_back({0, 0, width - 1, height - 1, 1.0f});

    accSteps = 0.0f;
    done = false;
  }

  bool update(float dt, uint32_t maxMs) override {
    if (done || pendingAreas.empty()) {
      done = true;
      return false;
    }

    uint32_t start = SDL_GetTicks();
    SDL_SetRenderTarget(renderer, texture);

    accSteps += dt * 50.0f;

    while (accSteps >= 1.0f && !pendingAreas.empty()) {
      if (SDL_GetTicks() - start >= maxMs)
        break;

      RectArea a = pendingAreas.front();
      pendingAreas.pop_front();

      int midX = (a.x1 + a.x2) / 2;
      int midY = (a.y1 + a.y2) / 2;

      if (a.x2 - a.x1 <= 1 && a.y2 - a.y1 <= 1)
        continue;

      float v1 = getGrid(a.x1, a.y1);
      float v2 = getGrid(a.x2, a.y1);
      float v3 = getGrid(a.x1, a.y2);
      float v4 = getGrid(a.x2, a.y2);

      float displacement = (randFloat() - 0.5f) * a.roughness;
      float centerV = (v1 + v2 + v3 + v4) / 4.0f + displacement;

      setGrid(midX, midY, centerV);
      setGrid(midX, a.y1, (v1 + v2) / 2.0f);
      setGrid(midX, a.y2, (v3 + v4) / 2.0f);
      setGrid(a.x1, midY, (v1 + v3) / 2.0f);
      setGrid(a.x2, midY, (v2 + v4) / 2.0f);

      Uint8 color = (Uint8)(std::fmax(0.0f, std::fmin(1.0f, centerV)) * 255);
      SDL_SetRenderDrawColor(renderer, color / 4, color / 2, color, 255);
      SDL_Rect r = {a.x1, a.y1, a.x2 - a.x1, a.y2 - a.y1};
      SDL_RenderFillRect(renderer, &r);

      float nextRough = a.roughness * 0.5f;
      pendingAreas.push_back({a.x1, a.y1, midX, midY, nextRough});
      pendingAreas.push_back({midX, a.y1, a.x2, midY, nextRough});
      pendingAreas.push_back({a.x1, midY, midX, a.y2, nextRough});
      pendingAreas.push_back({midX, midY, a.x2, a.y2, nextRough});

      accSteps -= 1.0f;
    }

    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    return !pendingAreas.empty();
  }

  const char *getName() const override { return "Plasma"; }

private:
  float randFloat() { return (float)rand() / RAND_MAX; }

  void setGrid(int x, int y, float v) { grid[y * width + x] = v; }
  float getGrid(int x, int y) { return grid[y * width + x]; }

  std::vector<float> grid;
  std::deque<RectArea> pendingAreas;
  float accSteps = 0.0f;
  bool done = false;
};