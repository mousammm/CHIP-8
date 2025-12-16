# Header Files Documentation

## Overview

The emulator uses two main header files that separate concerns:
- `chip8.h` - CHIP-8 emulation logic and state
- `sdl.h` - SDL graphics and input handling

---

## `sdl.h` - SDL Interface

### Included Headers

```c
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "chip8.h"
```

| Header | Purpose |
|--------|---------|
| `SDL2/SDL.h` | Main SDL2 library for graphics and input |
| `stdbool.h` | Boolean type support (`true`/`false`) |
| `chip8.h` | CHIP-8 emulator interface |

### Display Scaling

```c
#define SCALE_WINDOW 20
```

- **Purpose**: Scales the 64×32 CHIP-8 display for better visibility
- **Effect**: Creates a 1280×640 pixel window (64×20 = 1280, 32×20 = 640)

### SDL Structure

```c
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;
```

| Field | Type | Purpose |
|-------|------|---------|
| `window` | `SDL_Window*` | Main application window |
| `renderer` | `SDL_Renderer*` | 2D rendering context for the window |

### Function Declarations

| Function | Return Type | Parameters | Purpose |
|----------|-------------|------------|---------|
| `sdl_init` | `bool` | `sdl_t *sdl` | Initializes SDL subsystems and creates window/renderer |
| `sdl_cleanup` | `void` | `sdl_t *sdl` | Cleans up SDL resources and frees memory |
| `sdl_render_frame` | `void` | `sdl_t *sdl`, `chip8_t *chip8` | Renders the current CHIP-8 display state |
| `sdl_handle_inputs` | `bool` | `chip8_t *chip8` | Processes keyboard input and returns `false` to quit |

---

## `chip8.h` - CHIP-8 Emulator Core

### Included Headers

```c
#include <stdint.h> 
#include <stdbool.h> 
```

| Header | Purpose |
|--------|---------|
| `stdint.h` | Fixed-width integer types (`uint8_t`, `uint16_t`, etc.) |
| `stdbool.h` | Boolean type support |

### Memory and Display Constants

```c
#define MEMORY 4096          // 4KB of RAM
#define REGISTER 16          // 16 general-purpose registers (V0-VF)
#define STACK 16             // Stack depth for subroutine calls
#define DISPLAY_WIDTH 64     // CHIP-8 display width in pixels
#define DISPLAY_HEIGHT 32    // CHIP-8 display height in pixels
#define NUM_PAD 16           // Number of hexadecimal keypad keys (0-F)
```

### CHIP-8 Structure

```c
typedef struct {
    uint8_t memory[MEMORY];                // 4KB memory
    uint8_t V[REGISTER];                   // 16 general-purpose registers (V0-VF)
    uint16_t I;                            // Index register
    uint16_t pc;                           // Program counter
    uint16_t stack[STACK];                 // Call stack
    uint8_t sp;                            // Stack pointer
    uint8_t delay_timer;                   // Delay timer (60Hz)
    uint8_t sound_timer;                   // Sound timer (60Hz)
    uint8_t display[DISPLAY_WIDTH*DISPLAY_HEIGHT]; // Display buffer (0=off, 1=on)
    bool keypad[NUM_PAD];                  // Key states (true=pressed)
    bool draw_flag;                        // Flag indicating display needs update
} chip8_t;
```

### Function Declarations

| Function | Return Type | Parameters | Purpose |
|----------|-------------|------------|---------|
| `chip8_init` | `void` | `chip8_t *chip8` | Initializes CHIP-8 state and loads fontset |
| `chip8_load_rom` | `bool` | `chip8_t *chip8`, `const char *filename` | Loads ROM file into memory |
| `chip8_cycle` | `void` | `chip8_t *chip8` | Executes one CPU cycle (fetch, decode, execute) |
| `chip8_timers` | `void` | `chip8_t *chip8` | Updates delay and sound timers at 60Hz |

---
