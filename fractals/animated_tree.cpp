#include "fractal.h"
#include <cmath>

class AnimatedTree : public FractalFB {
public:
  explicit AnimatedTree(SDL_Renderer *r) : FractalFB(r) {}

  void reset() override {
    time = 0.0;
    clear();
  }

  bool update(float dt, uint32_t) override {
    time += dt * animSpeed;

    clear();
    SDL_SetRenderTarget(renderer, texture);

    double spread = angleBase + std::sin(time) * angleAmp;

    drawRecursive(width * 0.5, height * 0.98, startLen, -M_PI / 2.0, spread,
                  depth);

    SDL_SetRenderTarget(renderer, nullptr);
    return true;
  }

  const char *getName() const override { return "Animated Fractal Tree"; }

private:
  double time = 0.0;

  static constexpr int depth = 11;
  static constexpr double startLen = 180.0;
  static constexpr double lenShrink = 0.70;

  static constexpr double animSpeed = 2.0;
  static constexpr double angleBase = 0.5;
  static constexpr double angleAmp = 0.5;

  void drawRecursive(double x, double y, double len, double angle,
                     double spread, int d) {

    if (d == 0)
      return;

    double x2 = x + len * std::cos(angle);
    double y2 = y + len * std::sin(angle);

    int c = (d * 20 + int(time * 25)) & 0xFF;
    SDL_SetRenderDrawColor(renderer, c, (c + 80) & 0xFF, (c + 160) & 0xFF, 255);

    SDL_RenderDrawLine(renderer, int(x), int(y), int(x2), int(y2));

    double nextLen = len * lenShrink;

    drawRecursive(x2, y2, nextLen, angle - spread, spread, d - 1);

    drawRecursive(x2, y2, nextLen, angle + spread, spread, d - 1);
  }
};
