#pragma once
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

enum class FractalType {
  MANDELBROT,
  JULIA,
  PLASMA,
  KOCH,
  MENGER,
  PYTHAGORAS,
  SIERPINSKI,
  HILBERT_CURVE,
  ANIMATED_TREE,
  COUNT
};

class Fractal {
public:
  explicit Fractal(SDL_Renderer *r) : renderer(r) {}
  virtual ~Fractal() = default;

  virtual void resize(int w, int h) {
    width = w;
    height = h;
    reset();
  }

  virtual void reset() = 0;
  virtual bool update(float dt, uint32_t maxMs) = 0;
  virtual void render() = 0;
  virtual const char *getName() const = 0;

protected:
  SDL_Renderer *renderer{};
  int width{}, height{};
};

class FractalFB : public Fractal {
public:
  explicit FractalFB(SDL_Renderer *r) : Fractal(r) {}

  void resize(int w, int h) override {
    width = w;
    height = h;
    if (texture)
      SDL_DestroyTexture(texture);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_TARGET, width, height);
    reset();
  }

  virtual ~FractalFB() {
    if (texture)
      SDL_DestroyTexture(texture);
  }

  void render() override {
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  }

protected:
  SDL_Texture *texture = nullptr;

  void clear() {
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, nullptr);
  }
};
