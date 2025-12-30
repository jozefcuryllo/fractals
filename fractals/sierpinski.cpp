#include "fractal.h"
#include <algorithm>
#include <deque>

class Sierpinski : public FractalFB {
public:
  Sierpinski(SDL_Renderer *r) : FractalFB(r) {}

  struct Triangle {
    float x1, y1, x2, y2, x3, y3;
    int level;
  };

  void reset() override {
    clear();
    pendingTriangles.clear();

    float topX = width / 2.0f;
    float topY = 20.0f;
    float leftX = 20.0f;
    float leftY = height - 20.0f;
    float rightX = width - 20.0f;
    float rightY = height - 20.0f;

    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    drawTriangle({topX, topY, leftX, leftY, rightX, rightY, 0});
    SDL_SetRenderTarget(renderer, nullptr);

    pendingTriangles.push_back(
        {topX, topY, leftX, leftY, rightX, rightY, maxLevel});

    accSteps = 0.0f;
    done = false;
  }

  bool update(float dt, uint32_t maxMs) override {
    if (done || pendingTriangles.empty()) {
      done = true;
      return false;
    }

    uint32_t start = SDL_GetTicks();
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    accSteps += dt * 30.0f;

    while (accSteps >= 1.0f && !pendingTriangles.empty()) {
      if (SDL_GetTicks() - start >= maxMs)
        break;

      Triangle t = pendingTriangles.front();
      pendingTriangles.pop_front();

      if (t.level > 0) {

        float m12x = (t.x1 + t.x2) / 2.0f;
        float m12y = (t.y1 + t.y2) / 2.0f;
        float m23x = (t.x2 + t.x3) / 2.0f;
        float m23y = (t.y2 + t.y3) / 2.0f;
        float m31x = (t.x3 + t.x1) / 2.0f;
        float m31y = (t.y3 + t.y1) / 2.0f;

        drawTriangle({m12x, m12y, m23x, m23y, m31x, m31y, 0});

        pendingTriangles.push_back(
            {t.x1, t.y1, m12x, m12y, m31x, m31y, t.level - 1});
        pendingTriangles.push_back(
            {m12x, m12y, t.x2, t.y2, m23x, m23y, t.level - 1});
        pendingTriangles.push_back(
            {m31x, m31y, m23x, m23y, t.x3, t.y3, t.level - 1});
      }

      accSteps -= 1.0f;
    }

    SDL_SetRenderTarget(renderer, nullptr);
    return !pendingTriangles.empty();
  }

  const char *getName() const override { return "Sierpinski BFS"; }

private:
  void drawTriangle(const Triangle &t) {
    SDL_RenderDrawLine(renderer, (int)t.x1, (int)t.y1, (int)t.x2, (int)t.y2);
    SDL_RenderDrawLine(renderer, (int)t.x2, (int)t.y2, (int)t.x3, (int)t.y3);
    SDL_RenderDrawLine(renderer, (int)t.x3, (int)t.y3, (int)t.x1, (int)t.y1);
  }

  std::deque<Triangle> pendingTriangles;
  float accSteps = 0.0f;
  bool done = false;
  static constexpr int maxLevel = 10;
};