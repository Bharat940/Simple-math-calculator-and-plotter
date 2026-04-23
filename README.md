# Function Plotter

A powerful mathematical function plotting application with both GUI and command-line interfaces. Built with C++ and SDL2, it can parse, evaluate, and visualize complex mathematical expressions in real-time.

## Features

### GUI Mode
- Interactive plotting of mathematical functions
- Real-time zooming and panning with mouse wheel and keyboard
- Multiple function plotting with color-coded legends
- Display of roots (zeros) and extrema (minima/maxima)
- Tangent line visualization at cursor position
- Coordinate display at mouse position
- Grid and axis labels with adaptive scaling

### Command-Line Mode
- Expression evaluation: `plotter -e "sin(x) + cos(x)"`
- Root finding: `plotter -s "x^2 - 4"`
- Intersection finding: `plotter -i "x^2" "2*x + 1"`
- Explicit plotting: `plotter -p "x^3 - 3*x"`

## Supported Functions

### Trigonometric
- `sin(x)`, `cos(x)`, `tan(x)`
- `asin(x)`, `acos(x)`, `atan(x)`

### Hyperbolic
- `sinh(x)`, `cosh(x)`, `tanh(x)`

### Exponential & Logarithmic
- `exp(x)`, `log(x)`, `log10(x)`
- `log(x, base)` for arbitrary base logarithms

### Algebraic
- `sqrt(x)`, `abs(x)`
- `floor(x)`, `ceil(x)`
- `pow(x, y)`, `max(x, y)`, `min(x, y)`

### Constants
- `pi` (π ≈ 3.14159)
- `e` (Euler's number ≈ 2.71828)
- `phi` (Golden ratio ≈ 1.61803)

## Usage

### GUI Plotting
```bash
# Plot a single function
./plotter "x^2"

# Plot multiple functions
./plotter "sin(x), cos(x), x^2"

# Plot with custom initial view
./plotter "x^3 - 3*x" --range -5 5
```

### Command-Line Operations
```bash
# Evaluate expression at x=0
./plotter -e "sin(pi/2)"

# Find roots of equation f(x) = 0
./plotter -s "x^2 - 4"

# Find intersections of f(x) = g(x)
./plotter -i "x^2" "2*x + 1"

# Explicit GUI plot
./plotter -p "exp(-x^2)"
```

### Options
- `--range xmin xmax`: Set solving/plotting range
- `--step value`: Set solver step size
- `--precision value`: Set numeric precision
- `--zoom-step value`: Zoom sensitivity (1.0-2.0)
- `--zoom-min value`: Minimum zoom level
- `--zoom-max value`: Maximum zoom level
- `--scale mode`: Grid scaling (auto/fixed/loose/dense)
- `--verbose`: Detailed solver output

## Controls (GUI Mode)

| Control | Action |
|---------|--------|
| Mouse Wheel | Zoom in/out |
| Left/Right Arrows | Pan left/right |
| Up/Down Arrows | Pan up/down |
| `T` | Toggle tangent line |
| `G` | Toggle grid |
| `R` | Toggle roots display |
| `E` | Toggle extrema display |
| `Tab` | Cycle active function |
| `ESC` or Close | Exit |

## Building

### Prerequisites
- CMake 3.10+
- SDL2 development libraries
- SDL2_ttf development libraries
- C++17 compatible compiler

### Linux Build
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install cmake libsdl2-dev libsdl2-ttf-dev

# Build
mkdir build
cd build
cmake ..
make
```

## Architecture

The application is structured into several modules:

- **Tokenizer**: Lexical analysis of mathematical expressions
- **Parser**: Converts infix notation to postfix using shunting-yard algorithm
- **Evaluator**: Executes postfix expressions with function and constant support
- **Solver**: Numerical root finding using bisection and Newton-Raphson methods
- **Renderer**: SDL2-based graphics rendering with adaptive curve plotting
- **Numerical**: Derivative calculation and geometric computations

## Examples

### Basic Functions
```bash
./plotter "x^2, x^3, sqrt(x)"
./plotter "sin(x), cos(x), tan(x)"
```

### Complex Expressions
```bash
./plotter "sin(x^2) + cos(x^3)"
./plotter "exp(-x^2) * sin(10*x)"
```

### Parametric Plots (via expressions)
```bash
./plotter "x*sin(x), x*cos(x)"
```

## License

[MIT License](LICENSE)

## Contributing

Contributions welcome! Please ensure code follows the existing style and includes appropriate tests.

## Screenshots

*[Add screenshots here when available]*