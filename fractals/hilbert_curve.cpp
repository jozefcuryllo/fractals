#include "fractal.h"
#include <algorithm>
#include <cmath>
#include <vector>

class HilbertCurve : public FractalFB {
public:
  HilbertCurve(SDL_Renderer *r) : FractalFB(r) {}

  struct Point {
    float x, y;
  };

  void reset() override {
    clear();
    level = 1;
    accSteps = 0.0f;
    currentDrawIdx = 0;
    done = false;

    generatePath();
  }

  bool update(float dt, uint32_t maxMs) override {
    if (done) {
      drawToScreen();
      return false;
    }

    uint32_t start = SDL_GetTicks();
    SDL_SetRenderTarget(renderer, texture);

    float speed = 30.0f + (std::pow(4, level) * 0.5f);
    accSteps += dt * speed;

    while (accSteps >= 1.0f) {
      if (SDL_GetTicks() - start >= maxMs)
        break;

      if (currentDrawIdx < (int)path.size() - 1) {

        float progress = (float)currentDrawIdx / path.size();
        setRainbowColor(progress);

        Point p1 = path[currentDrawIdx];
        Point p2 = path[currentDrawIdx + 1];

        SDL_RenderDrawLine(renderer, (int)p1.x, (int)p1.y, (int)p2.x,
                           (int)p2.y);

        currentDrawIdx++;
        accSteps -= 1.0f;
      } else {

        if (level < MAX_LEVEL) {
          level++;
          generatePath();

          SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
          SDL_RenderClear(renderer);

          currentDrawIdx = 0;
          accSteps = 0.0f;
          break;
        } else {
          done = true;
          break;
        }
      }
    }

    SDL_SetRenderTarget(renderer, nullptr);
    drawToScreen();
    return !done;
  }

  const char *getName() const override { return "Hilbert Curve"; }

private:
  void setRainbowColor(float p) {

    Uint8 r = (Uint8)(std::sin(p * 6.28f) * 127 + 128);
    Uint8 g = (Uint8)(std::sin(p * 6.28f + 2.0f) * 127 + 128);
    Uint8 b = (Uint8)(std::sin(p * 6.28f + 4.0f) * 127 + 128);
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
  }

  void generatePath() {
    path.clear();
    int n = (int)std::pow(2, level);
    int totalPoints = n * n;

    float margin = 40.0f;
    float availableSize = std::min(width, height) - (margin * 2.0f);
    float step = availableSize / (n - 1);

    float offsetX = (width - (availableSize)) / 2.0f;
    float offsetY = (height - (availableSize)) / 2.0f;

    for (int i = 0; i < totalPoints; i++) {
      Point p = d2xy(n, i);
      path.push_back({p.x * step + offsetX, p.y * step + offsetY});
    }
  }

  Point d2xy(int n, int d) {
    Point p = {0, 0};
    int t = d;
    for (int s = 1; s < n; s <<= 1) {
      int rx = 1 & (t / 2);
      int ry = 1 & (t ^ rx);
      rot(s, &p.x, &p.y, rx, ry);
      p.x += s * (float)rx;
      p.y += s * (float)ry;
      t /= 4;
    }
    return p;
  }

  void rot(int n, float *x, float *y, int rx, int ry) {
    if (ry == 0) {
      if (rx == 1) {
        *x = (float)n - 1.0f - *x;
        *y = (float)n - 1.0f - *y;
      }
      std::swap(*x, *y);
    }
  }

  void drawToScreen() { SDL_RenderCopy(renderer, texture, nullptr, nullptr); }

  std::vector<Point> path;
  int level = 1;
  int currentDrawIdx = 0;
  float accSteps = 0.0f;
  const int MAX_LEVEL = 10;
  bool done = false;
};