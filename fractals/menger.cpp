#include "fractal.h"
#include <algorithm>
#include <vector>

class Menger : public FractalFB {
public:
  Menger(SDL_Renderer *r) : FractalFB(r) {}

  struct Cube {
    int x, y, size;
  };

  void reset() override {
    clear();

    int s = std::min(width, height) - 40;
    int x = (width - s) / 2;
    int y = (height - s) / 2;

    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect mainRect{x, y, s, s};
    SDL_RenderFillRect(renderer, &mainRect);

    currentLevelCubes.clear();
    nextLevelCubes.clear();

    currentLevelCubes.push_back({x, y, s});

    level = 0;
    accSteps = 0.0f;
    done = false;
    SDL_SetRenderTarget(renderer, nullptr);
  }

  bool update(float dt, uint32_t maxMs) override {
    if (done) {
      drawToScreen();
      return false;
    }

    uint32_t start = SDL_GetTicks();
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    accSteps += dt * 40.0f;

    while (accSteps >= 1.0f) {
      if (SDL_GetTicks() - start >= maxMs)
        break;

      if (currentLevelCubes.empty()) {

        if (nextLevelCubes.empty() || level >= MAX_LEVEL) {
          done = true;
          break;
        }
        currentLevelCubes = std::move(nextLevelCubes);
        nextLevelCubes.clear();
        level++;
      }

      Cube c = currentLevelCubes.back();
      currentLevelCubes.pop_back();

      int ns = c.size / 3;
      if (ns >= 1) {

        SDL_Rect hole{c.x + ns, c.y + ns, ns, ns};
        SDL_RenderFillRect(renderer, &hole);

        for (int i = 0; i < 3; ++i) {
          for (int j = 0; j < 3; ++j) {
            if (!(i == 1 && j == 1)) {
              nextLevelCubes.push_back({c.x + i * ns, c.y + j * ns, ns});
            }
          }
        }
      }
      accSteps -= 1.0f;
    }

    SDL_SetRenderTarget(renderer, nullptr);
    drawToScreen();
    return !done;
  }

  const char *getName() const override { return "Menger"; }

private:
  void drawToScreen() {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  }

  std::vector<Cube> currentLevelCubes;
  std::vector<Cube> nextLevelCubes;
  float accSteps = 0.0f;
  int level = 0;
  static constexpr int MAX_LEVEL = 10;
  bool done = false;
};