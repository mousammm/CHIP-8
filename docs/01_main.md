# CHIP-8 Emulator - `main.c`

## Overview
This is the main entry point for a CHIP-8 emulator implementation using SDL for graphics and input handling.

## File Inclusion

```c
#include <stdio.h>   
#include "sdl.h"
#include "chip8.h"
```

| Header | Purpose |
|--------|---------|
| `stdio.h` | Standard input/output operations (e.g., `printf`) |
| `sdl.h` | SDL graphics and input handling interface |
| `chip8.h` | Core CHIP-8 emulation logic |

## Main Function

### Signature
```c
int main(int argc, char **argv)
```

### Argument Validation
```c
if (argc < 2) return 1;
```
- Validates that a ROM file path was provided as a command-line argument
- Returns error code 1 if no argument is provided
- *Note: Could be enhanced with usage instructions*

---

## Emulator Initialization

### CHIP-8 Setup
```c
chip8_t chip8 = {0};
chip8_init(&chip8);
```
- Creates and zero-initializes a `chip8_t` structure
- Calls initialization function with pointer to the structure
- *Pointer knowledge is helpful for understanding this implementation*

### ROM Loading
```c
if (!chip8_load_rom(&chip8, argv[1])) return 1;
```
- Loads the ROM file specified as the first command-line argument
- Returns error code 1 if ROM loading fails
- *Note: Error feedback could be improved*

### SDL Initialization
```c
sdl_t sdl = {0};
if (!sdl_init(&sdl)) return 1;
```
- Creates and zero-initializes an `sdl_t` structure
- Initializes SDL subsystems (video, audio, etc.)
- Returns error code 1 if initialization fails

---

## Main Emulation Loop

```c
while (sdl_handle_inputs(&chip8)) {
    chip8_cycle(&chip8);
    chip8_timers(&chip8);
    
    if (chip8.draw_flag) {
        sdl_render_frame(&sdl, &chip8);
    }
    
    SDL_Delay(2);
}
```

| Component | Purpose |
|-----------|---------|
| `sdl_handle_inputs()` | Processes user input; loop continues while this returns `true` |
| `chip8_cycle()` | Executes one CHIP-8 CPU cycle |
| `chip8_timers()` | Updates delay and sound timers |
| `chip8.draw_flag` | Flag indicating the screen needs redrawing |
| `sdl_render_frame()` | Renders the current frame to the screen |
| `SDL_Delay(2)` | Controls emulation speed (~500 FPS) |

*Note: Consider implementing frame rate control via macros or dynamic calculation instead of hardcoded delay*

---

## Cleanup and Termination

```c
sdl_cleanup(&sdl);
return 0;
```

| Function | Purpose |
|----------|---------|
| `sdl_cleanup()` | Frees SDL resources and heap memory |
| `return 0;` | Indicates successful program execution |

**Important:** SDL uses heap memory which must be explicitly freed to prevent memory leaks.

---

## Usage
```bash
./chip8_emulator <path_to_rom_file>
```

## Key Considerations
1. Minimal error messaging (could be enhanced)
2. Fixed frame rate delay (could be made configurable)
3. Basic input validation
4. Proper resource cleanup on exit

---
