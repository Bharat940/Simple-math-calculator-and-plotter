#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "math/expression.h"
#include "math/geometry.h"

enum class ScaleMode
{
    Auto,
    Fixed,
    Loose,
    Dense
};

void setScaleMode(ScaleMode mode);

int mapX(double x, int width, double xmin, double xmax);
int mapY(double y, int height, double ymin, double ymax);

void drawAxes(
    SDL_Renderer *renderer,
    int width,
    int height,
    double xmin,
    double xmax,
    double ymin,
    double ymax);

void drawGrid(
    SDL_Renderer *renderer,
    int width,
    int height,
    double xmin,
    double xmax,
    double ymin,
    double ymax);

// Function
void drawFunction(
    SDL_Renderer *renderer,
    const Expression &expr,
    size_t colorIndex,
    bool isActive,
    int width,
    int height,
    double xmin,
    double xmax,
    double ymin,
    double ymax);

void drawAxisLabels(
    SDL_Renderer *renderer,
    TTF_Font *font,
    int width,
    int height,
    double xmin,
    double xmax,
    double ymin,
    double ymax);

void drawLine(
    SDL_Renderer *renderer,
    const Line &line,
    int width,
    int height,
    double xmin,
    double xmax,
    double ymin,
    double ymax);

void drawRoots(
    SDL_Renderer *renderer,
    const std::vector<double> &roots,
    int width,
    int height,
    double xmin,
    double xmax,
    double ymin,
    double ymax);

void drawExtrema(
    SDL_Renderer *renderer,
    const Expression &expr,
    const std::vector<double> &extrema,
    int width,
    int height,
    double xmin,
    double xmax,
    double ymin,
    double ymax);

void drawLegend(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const std::vector<std::string> &labels,
    size_t activeIndex);

double screenToMathX(
    int px,
    int width,
    double xmin,
    double xmax);

double screenToMathY(
    int py,
    int height,
    double ymin,
    double ymax);
