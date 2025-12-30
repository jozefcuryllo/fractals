#pragma once
#include "fractal.h"
#include <memory>

std::unique_ptr<FractalFB> createFractal(FractalType type, SDL_Renderer *r);
const char *getFractalName(FractalType type);