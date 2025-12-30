#include "fractal.h"
#include <cmath>
#include <vector>

class Pythagoras : public FractalFB {
public:
  Pythagoras(SDL_Renderer *r) : FractalFB(r) {}

  void reset() override {
    clear();
    current.clear();
    next.clear();

    current.push_back({(float)width / 2.0f, (float)height - 10.0f,
                       (float)height / 4.0f, -M_PI / 2.0f, maxDepth});

    done = false;
  }

  bool update(float dt, uint32_t maxMs) override {
    if (done || current.empty())
      return false;

    uint32_t start = SDL_GetTicks();
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    levelAccumulator += dt * 10.0f;

    while (levelAccumulator >= 1.0f && !current.empty()) {

      if (SDL_GetTicks() - start >= maxMs)
        break;

      next.clear();
      for (const Node &n : current) {
        if (n.depth <= 0 || n.len < 1.4f)
          continue;

        float x2 = n.x + n.len * std::cos(n.angle);
        float y2 = n.y + n.len * std::sin(n.angle);
        SDL_RenderDrawLine(renderer, (int)n.x, (int)n.y, (int)x2, (int)y2);

        next.push_back({x2, y2, n.len * 0.7f, n.angle - 0.4f, n.depth - 1});
        next.push_back({x2, y2, n.len * 0.7f, n.angle + 0.4f, n.depth - 1});
      }

      current.swap(next);
      levelAccumulator -= 1.0f;
    }

    SDL_SetRenderTarget(renderer, nullptr);

    if (current.empty())
      done = true;
    return !done;
  }

  const char *getName() const override { return "Pythagoras Tree"; }

private:
  struct Node {
    float x, y;
    float len;
    float angle;
    int depth;
  };

  std::vector<Node> current;
  std::vector<Node> next;

  bool done = false;
  static constexpr int maxDepth = 60;
  float levelAccumulator = 0.0f;
};
