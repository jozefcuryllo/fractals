#include "factory.h"

#include "animated_tree.cpp"
#include "hilbert_curve.cpp"
#include "julia.cpp"
#include "koch_snowflake.cpp"
#include "mandelbrot.cpp"
#include "menger.cpp"
#include "plasma.cpp"
#include "pythagoras.cpp"
#include "sierpinski.cpp"

std::unique_ptr<FractalFB> createFractal(FractalType t, SDL_Renderer *r) {
  switch (t) {
  case FractalType::MANDELBROT:
    return std::make_unique<Mandelbrot>(r);
  case FractalType::JULIA:
    return std::make_unique<Julia>(r);
  case FractalType::PLASMA:
    return std::make_unique<Plasma>(r);
  case FractalType::KOCH:
    return std::make_unique<Koch>(r);
  case FractalType::MENGER:
    return std::make_unique<Menger>(r);
  case FractalType::PYTHAGORAS:
    return std::make_unique<Pythagoras>(r);
  case FractalType::SIERPINSKI:
    return std::make_unique<Sierpinski>(r);
  case FractalType::HILBERT_CURVE:
    return std::make_unique<HilbertCurve>(r);
  case FractalType::ANIMATED_TREE:
    return std::make_unique<AnimatedTree>(r);
  default:
    return {};
  }
}

const char *getFractalName(FractalType t) {
  static const char *names[] = {
      "Mandelbrot",     "Julia",         "Plasma",
      "Koch Snowflake", "Menger Sponge", "Pythagoras Tree",
      "Sierpinski",     "Hilbert Curve", "Animated Tree"};
  return names[(int)t];
}
