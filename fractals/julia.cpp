#include "fractal.h"

class Julia : public FractalFB {
public:
  Julia(SDL_Renderer *r) : FractalFB(r) {}

  void reset() override {
    clear();
    iter = 0;
    iterAccumulator = 0.0f;
    alive = width * height;

    zx.assign(width * height, 0.0);
    zy.assign(width * height, 0.0);
    escaped.assign(width * height, false);

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        int i = y * width + x;
        zx[i] = (x - width / 2.0) * 4.0 / width;
        zy[i] = (y - height / 2.0) * 4.0 / width;
      }
    }
  }

  bool update(float dt, uint32_t maxMs) override {
    if (alive <= 0 || iter >= maxIter)
      return false;

    uint32_t start = SDL_GetTicks();
    SDL_SetRenderTarget(renderer, texture);

    iterAccumulator += dt * 60.0f;

    while (iterAccumulator >= 1.0f && alive > 0 && iter < maxIter) {

      if (SDL_GetTicks() - start >= maxMs)
        break;

      for (int i = 0; i < width * height; ++i) {
        if (escaped[i])
          continue;

        double x = zx[i];
        double y = zy[i];

        const double cx = -0.7;
        const double cy = 0.27015;

        double nx = x * x - y * y + cx;
        double ny = 2.0 * x * y + cy;

        zx[i] = nx;
        zy[i] = ny;

        if (nx * nx + ny * ny > 4.0) {
          escaped[i] = true;
          alive--;

          int px = i % width;
          int py = i / width;

          Uint8 r = (iter * 7) % 255;
          Uint8 g = (iter * 3) % 255;
          Uint8 b = (iter * 11) % 255;

          SDL_SetRenderDrawColor(renderer, r, g, b, 255);
          SDL_RenderDrawPoint(renderer, px, py);
        }
      }

      iter++;
      iterAccumulator -= 1.0f;
    }

    SDL_SetRenderTarget(renderer, nullptr);
    return alive > 0 && iter < maxIter;
  }

  const char *getName() const override { return "Julia"; }

private:
  std::vector<double> zx, zy;
  std::vector<bool> escaped;
  float iterAccumulator = 0.0f;

  int iter = 0;
  int alive = 0;
  static constexpr int maxIter = 500;
};