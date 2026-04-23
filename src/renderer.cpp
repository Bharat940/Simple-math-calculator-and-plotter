#include "renderer.h"
#include "math/constants.h"
#include <string>
#include <cmath>
#include <SDL_ttf.h>

static SDL_Color PALETTE[] = {
	{0, 255, 0, 255},
	{255, 0, 0, 255},
	{0, 0, 255, 255},
	{255, 165, 0, 255},
	{255, 0, 255, 255}};

constexpr size_t PALETTE_SIZE = sizeof(PALETTE) / sizeof(PALETTE[0]);

static ScaleMode gScaleMode = ScaleMode::Auto;

void setScaleMode(ScaleMode mode)
{
	gScaleMode = mode;
}

static void drawAdaptiveSegment(
	SDL_Renderer *renderer,
	const Expression &expr,
	double x1, double y1,
	double x2, double y2,
	int width,
	int height,
	double xmin,
	double xmax,
	double ymin,
	double ymax,
	int depth)
{
	// Safety limits
	if (depth <= 0)
	{
		int px1 = mapX(x1, width, xmin, xmax);
		int py1 = mapY(y1, height, ymin, ymax);
		int px2 = mapX(x2, width, xmin, xmax);
		int py2 = mapY(y2, height, ymin, ymax);
		SDL_RenderDrawLine(renderer, px1, py1, px2, py2);
		return;
	}

	double midX = (x1 + x2) * 0.5;
	double midY = expr.eval(midX);

	if (!std::isfinite(midY))
	{
		return;
	}

	double linearMidY = (y1 + y2) * 0.5;

	// Convert error to screen space
	int pyMidActual = mapY(midY, height, ymin, ymax);
	int pyMidLinear = mapY(linearMidY, height, ymin, ymax);

	if (std::abs(pyMidActual - pyMidLinear) > 1)
	{
		drawAdaptiveSegment(
			renderer, expr,
			x1, y1, midX, midY,
			width, height,
			xmin, xmax, ymin, ymax,
			depth - 1);
		drawAdaptiveSegment(
			renderer, expr,
			midX, midY, x2, y2,
			width, height,
			xmin, xmax, ymin, ymax,
			depth - 1);
	}
	else
	{
		int px1 = mapX(x1, width, xmin, xmax);
		int py1 = mapY(y1, height, ymin, ymax);
		int px2 = mapX(x2, width, xmin, xmax);
		int py2 = mapY(y2, height, ymin, ymax);
		SDL_RenderDrawLine(renderer, px1, py1, px2, py2);
	}
}

int mapX(double x, int width, double xmin, double xmax)
{
	return (int)((x - xmin) / (xmax - xmin) * width);
}

int mapY(double y, int height, double ymin, double ymax)
{
	return (int)(height - (y - ymin) / (ymax - ymin) * height);
}

void drawAxes(
	SDL_Renderer *renderer,
	int width,
	int height,
	double xmin,
	double xmax,
	double ymin,
	double ymax)
{
	int originX = mapX(0, width, xmin, xmax);
	int originY = mapY(0, height, ymin, ymax);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_RenderDrawLine(renderer, 0, originY, width, originY);
	SDL_RenderDrawLine(renderer, originX, 0, originX, height);
}

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
	double ymax)
{
	SDL_Color c = PALETTE[colorIndex % PALETTE_SIZE];

	if (isActive)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
	}

	int baseSamples = width / 8;
	double step = (xmax - xmin) / baseSamples;

	double x1 = xmin;
	double y1 = expr.eval(x1);

	for (int i = 1; i <= baseSamples; ++i)
	{
		double x2 = xmin + i * step;
		double y2 = expr.eval(x2);

		if (std::isfinite(y1) && std::isfinite(y2))
		{
			double slope = std::abs(y2 - y1) / step;

			if (slope > EPS_DISCONTINUITY)
			{
				x1 = x2;
				y1 = y2;
				continue;
			}

			drawAdaptiveSegment(
				renderer, expr,
				x1, y1,
				x2, y2,
				width, height,
				xmin, xmax, ymin, ymax,
				10 // recursion depth
			);
		}

		x1 = x2;
		y1 = y2;
	}
}

void drawLine(
	SDL_Renderer *renderer,
	const Line &line,
	int width,
	int height,
	double xmin,
	double xmax,
	double ymin,
	double ymax)
{
	double x1 = xmin;
	double y1 = line.m * x1 + line.b;

	double x2 = xmax;
	double y2 = line.m * x2 + line.b;

	if (!std::isfinite(y1) || !std::isfinite(y2))
	{
		return;
	}

	int px1 = mapX(x1, width, xmin, xmax);
	int py1 = mapY(y1, height, ymin, ymax);
	int px2 = mapX(x2, width, xmin, xmax);
	int py2 = mapY(y2, height, ymin, ymax);

	SDL_RenderDrawLine(renderer, px1, py1, px2, py2);
}

void drawRoots(
	SDL_Renderer *renderer,
	const std::vector<double> &roots,
	int width,
	int height,
	double xmin,
	double xmax,
	double ymin,
	double ymax)
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	for (double r : roots)
	{
		int px = mapX(r, width, xmin, xmax);
		int py = mapY(0, height, ymin, ymax);

		// small cross
		SDL_RenderDrawLine(renderer, px - 5, py, px + 5, py);
		SDL_RenderDrawLine(renderer, px, py - 5, px, py + 5);
	}
}

void drawExtrema(
	SDL_Renderer *renderer,
	const Expression &expr,
	const std::vector<double> &extrema,
	int width,
	int height,
	double xmin,
	double xmax,
	double ymin,
	double ymax)
{
	SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);

	for (double x : extrema)
	{
		double y = expr.eval(x);

		if (!std::isfinite(y))
		{
			continue;
		}

		int px = mapX(x, width, xmin, xmax);
		int py = mapY(y, height, ymin, ymax);

		SDL_Rect r{px - 4, py - 4, 8, 8};
		SDL_RenderFillRect(renderer, &r);
	}
}

static double niceStep(double range, int targetLines)
{
	if (range <= 0 || targetLines <= 0)
	{
		return 1.0;
	}

	switch (gScaleMode)
	{
	case ScaleMode::Loose:
		targetLines /= 2;
		break;
	case ScaleMode::Dense:
		targetLines *= 2;
		break;
	case ScaleMode::Fixed:
		targetLines = 10;
		break;
	case ScaleMode::Auto:
	default:
		break;
	}

	double raw = range / targetLines;
	double exp = std::floor(std::log10(raw));
	double base = std::pow(10.0, exp);

	double f = raw / base;

	if (f < 1.5)
	{
		return 1 * base;
	}
	if (f < 3.0)
	{
		return 2 * base;
	}
	if (f < 7.0)
	{
		return 5 * base;
	}
	return 10 * base;
}

void drawGrid(
	SDL_Renderer *renderer,
	int width,
	int height,
	double xmin,
	double xmax,
	double ymin,
	double ymax)
{
	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);

	double xRange = xmax - xmin;
	double yRange = ymax - ymin;

	if (xRange <= 0 || yRange <= 0)
		return;

	int targetX = width / 120;
	int targetY = height / 80;

	double xStep = niceStep(xRange, targetX);
	double yStep = niceStep(yRange, targetY);

	double xStart = std::floor(xmin / xStep) * xStep;
	for (double x = xStart; x <= xmax; x += xStep)
	{
		int px = mapX(x, width, xmin, xmax);
		SDL_RenderDrawLine(renderer, px, 0, px, height);
	}

	double yStart = std::floor(ymin / yStep) * yStep;
	for (double y = yStart; y <= ymax; y += yStep)
	{
		int py = mapY(y, height, ymin, ymax);
		SDL_RenderDrawLine(renderer, 0, py, width, py);
	}
}

void drawAxisLabels(
	SDL_Renderer *renderer,
	TTF_Font *font,
	int width,
	int height,
	double xmin,
	double xmax,
	double ymin,
	double ymax)
{
	SDL_Color color = {200, 200, 200, 255};

	int originX = mapX(0, width, xmin, xmax);
	int originY = mapY(0, height, ymin, ymax);

	double xRange = xmax - xmin;
	double yRange = ymax - ymin;

	if (xRange <= 0 || yRange <= 0)
		return;

	// IMPORTANT: use SAME spacing as grid
	double xStep = niceStep(xRange, width / 120);
	double yStep = niceStep(yRange, height / 80);

	// ---- X AXIS LABELS ----
	double xStart = std::ceil(xmin / xStep) * xStep;

	for (double x = xStart; x <= xmax; x += xStep)
	{
		if (std::abs(x) < 1e-9)
			continue; // skip origin label

		int px = mapX(x, width, xmin, xmax);
		if (px < 20 || px > width - 20)
			continue;

		std::string text = std::to_string(x);
		auto dot = text.find('.');
		if (dot != std::string::npos)
			text = text.substr(0, dot + 3);

		SDL_Surface *surface =
			TTF_RenderText_Solid(font, text.c_str(), color);

		if (!surface)
			continue;

		SDL_Texture *texture =
			SDL_CreateTextureFromSurface(renderer, surface);

		if (!texture)
		{
			SDL_FreeSurface(surface);
			continue;
		}

		SDL_Rect dst{
			px - surface->w / 2,
			originY + 4,
			surface->w,
			surface->h};

		SDL_RenderCopy(renderer, texture, nullptr, &dst);
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}

	// ---- Y AXIS LABELS ----
	double yStart = std::ceil(ymin / yStep) * yStep;

	for (double y = yStart; y <= ymax; y += yStep)
	{
		if (std::abs(y) < 1e-9)
			continue; // skip origin label

		int py = mapY(y, height, ymin, ymax);
		if (py < 20 || py > height - 20)
			continue;

		std::string text = std::to_string(y);
		auto dot = text.find('.');
		if (dot != std::string::npos)
			text = text.substr(0, dot + 3);

		SDL_Surface *surface =
			TTF_RenderText_Solid(font, text.c_str(), color);

		if (!surface)
			continue;

		SDL_Texture *texture =
			SDL_CreateTextureFromSurface(renderer, surface);

		if (!texture)
		{
			SDL_FreeSurface(surface);
			continue;
		}

		SDL_Rect dst{
			originX + 6,
			py - surface->h / 2,
			surface->w,
			surface->h};

		SDL_RenderCopy(renderer, texture, nullptr, &dst);
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}
}

void drawLegend(
	SDL_Renderer *renderer,
	TTF_Font *font,
	const std::vector<std::string> &labels,
	size_t activeIndex)
{
	SDL_Color inactiveColor = {180, 180, 180, 255};
	SDL_Color activeColor = {255, 255, 255, 255};

	int x = 10;
	int y = 10;
	int lineHeight = 18;

	for (size_t i = 0; i < labels.size(); i++)
	{
		SDL_Color color = (i == activeIndex) ? activeColor : inactiveColor;

		SDL_Surface *surface = TTF_RenderText_Solid(font, labels[i].c_str(), color);

		if (!surface)
		{
			continue;
		}

		SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_Rect dst{
			x,
			y + static_cast<int>(i) * lineHeight,
			surface->w,
			surface->h};

		SDL_RenderCopy(renderer, texture, nullptr, &dst);

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}
}

double screenToMathX(
	int px,
	int width,
	double xmin,
	double xmax)
{
	return xmin + (double)px / width * (xmax - xmin);
}

double screenToMathY(
	int py,
	int height,
	double ymin,
	double ymax)
{
	return ymin + (1.0 - (double)py / height) * (ymax - ymin);
}
