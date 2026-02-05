# Bounce
(Created in SDL2)
A visualization of colored squares that travel horizontally accross the screen according to a user-defined function.
## Building
Requires SDL2 and CMake.

```sh
cmake -B build
cmake --build build
```

## Running
```sh
cd build
./app
```

Press `ESC` or close the window to quit.

## Configuration

Edit `config.txt` in the project root to control the simulation.

```
# Number of rectangles (MAX 500)
n_rects = 100

# Color spectrum range (0.0 - 1.0 or rainbow)
color_range = 0.6

# dx expression - evaluated per frame
dx = 10 * sin((n - i) / n * 2 * pi + t)
```

### Parameters

| Key           | Description                                      |
|---------------|--------------------------------------------------|
| `n_rects`     | Number of rectangles (stacked vertically)        |
| `color_range` | Rainbow color spread, from `0.0` to `1.0`       |
| `dx`          | Math expression for horizontal velocity per frame|

### Expression variables

| Variable | Description                        |
|----------|------------------------------------|
| `i`      | Rectangle index (0 to n-1)        |
| `n`      | Total number of rectangles         |
| `t`      | Elapsed time in seconds            |

### Available functions and constants

- **Functions:** `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `sqrt`, `floor`, `ceil`
- **Constants:** `pi`, `e`
- **Operators:** `+`, `-`, `*`, `/`, `^` (exponentiation)
- Implicit multiplication is supported (e.g. `2pi` = `2 * pi`)

### Example expressions

```
# Sine wave that animates over time
dx = 10 * sin((n - i) / n * 2 * pi)

# Rectangles accelerate based on index
dx = t / 50 * i

# Cosine modulated by position
dx = 5 * cos(t) * i / n

# Damped wave
dx = 15 * sin(i / n * 8 * pi) * e^(-(i / n) * 3)
```
