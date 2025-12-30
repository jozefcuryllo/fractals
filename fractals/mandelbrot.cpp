#include "fractal.h"

class Mandelbrot : public FractalFB {
public:
  Mandelbrot(SDL_Renderer *r) : FractalFB(r) {}

  void reset() override {
    iter = 1;
    iterAcc = 0.0f;
    clear();
  }

  bool update(float dt, uint32_t maxMs) override {
    if (iter > 256)
      return false;

    uint32_t start = SDL_GetTicks();
    SDL_SetRenderTarget(renderer, texture);

    iterAcc += dt * 100.0f;

    while (iterAcc >= 1.0f && iter <= 256) {

      if (SDL_GetTicks() - start >= maxMs)
        break;

      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
          double cx = (x - width / 2.0) * 4.0 / width;
          double cy = (y - height / 2.0) * 4.0 / width;

          double zx = 0.0, zy = 0.0;
          int i = 0;

          while (zx * zx + zy * zy < 4.0 && i < iter) {
            double t = zx * zx - zy * zy + cx;
            zy = 2 * zx * zy + cy;
            zx = t;
            ++i;
          }

          if (zx * zx + zy * zy >= 4.0 && i == iter) {
            Uint8 c = Uint8(255 * iter / 256.0);
            SDL_SetRenderDrawColor(renderer, c, c, c, 255);
            SDL_RenderDrawPoint(renderer, x, y);
          }
        }
      }

      iter++;
      iterAcc -= 1.0f;
    }

    SDL_SetRenderTarget(renderer, nullptr);
    return iter <= 256;
  }

  const char *getName() const override { return "Mandelbrot"; }

private:
  int iter = 1;
  float iterAcc = 0.0f;
};
