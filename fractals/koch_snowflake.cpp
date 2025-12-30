#include "fractal.h"
#include <cmath>
#include <vector>

class Koch : public FractalFB {
public:
  Koch(SDL_Renderer *r) : FractalFB(r) {}

  struct Point {
    float x, y;
  };

  struct Segment {
    Point a, b;
  };

  void reset() override {
    if (texture) {
      SDL_DestroyTexture(texture);
      texture = nullptr;
    }

    segs.clear();
    step = 0;
    accum = 0.0f;
    finished = false;

    float side = std::min(width, height) * 0.6f;
    float h = side * std::sqrt(3.0f) / 2.0f;
    float centerX = width * 0.5f;
    float centerY = height * 0.5f;

    Point top = {centerX, centerY - h * 0.5f};
    Point left = {centerX - side * 0.5f, centerY + h * 0.5f};
    Point right = {centerX + side * 0.5f, centerY + h * 0.5f};

    segs.push_back({top, right});
    segs.push_back({right, left});
    segs.push_back({left, top});

    redraw();
  }

  bool update(float dt, uint32_t maxMs) override {
    if (finished)
      return false;

    accum += dt;
    if (accum < 0.8f)
      return true;

    accum = 0.0f;
    if (step < 8) {
      subdivide();
      step++;
      redraw();
    } else {
      finished = true;
    }

    return !finished;
  }

  void render() override {
    if (texture) {
      SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    }
  }

  const char *getName() const override { return "Koch Snowflake"; }

private:
  void subdivide() {
    std::vector<Segment> next;
    next.reserve(segs.size() * 4);

    float hcoeff = std::sqrt(3.0f) / 6.0f;

    for (const auto &seg : segs) {
      float dx = seg.b.x - seg.a.x;
      float dy = seg.b.y - seg.a.y;

      Point p1 = seg.a;
      Point p2 = {seg.a.x + dx / 3.0f, seg.a.y + dy / 3.0f};
      Point p4 = {seg.a.x + 2.0f * dx / 3.0f, seg.a.y + 2.0f * dy / 3.0f};
      Point p5 = seg.b;

      Point p3 = {(seg.a.x + seg.b.x) * 0.5f + dy * hcoeff,
                  (seg.a.y + seg.b.y) * 0.5f - dx * hcoeff};

      next.push_back({p1, p2});
      next.push_back({p2, p3});
      next.push_back({p3, p4});
      next.push_back({p4, p5});
    }

    segs = std::move(next);
  }

  void redraw() {
    if (texture)
      SDL_DestroyTexture(texture);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_TARGET, width, height);

    if (!texture)
      return;

    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 220, 240, 255, 255);
    for (const auto &seg : segs) {
      SDL_RenderDrawLineF(renderer, seg.a.x, seg.a.y, seg.b.x, seg.b.y);
    }

    SDL_SetRenderTarget(renderer, nullptr);
  }

  std::vector<Segment> segs;
  float accum = 0.0f;
  int step = 0;
  bool finished = false;
  SDL_Texture *texture = nullptr;
};