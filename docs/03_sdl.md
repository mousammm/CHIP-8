# SDL Implementation (`sdl.c`)

## Overview
This file implements the SDL graphics and input handling interface for the CHIP-8 emulator. It provides window management, display rendering, and keyboard input processing.

## Headers

```c
#include <stdio.h>
#include <stdlib.h>
#include "sdl.h"
```

| Header | Purpose |
|--------|---------|
| `stdio.h` | Standard I/O for error messages |
| `stdlib.h` | Standard library functions |
| `sdl.h` | SDL interface |

---

## Function: `bool sdl_init(sdl_t *sdl)`

### Purpose
Initializes SDL subsystems and creates the application window and renderer.

### Implementation

```c
// Initialize SDL with video subsystem
if ((SDL_Init(SDL_INIT_VIDEO) != 0)) return false;

// Create window
sdl->window = SDL_CreateWindow(
    "Chip8",                          // Window title
    SDL_WINDOWPOS_CENTERED,           // X position (centered)
    SDL_WINDOWPOS_CENTERED,           // Y position (centered)
    SCALE_WINDOW * DISPLAY_WIDTH,     // Window width (1280 pixels)
    SCALE_WINDOW * DISPLAY_HEIGHT,    // Window height (640 pixels)
    SDL_WINDOW_SHOWN                  // Window creation flags
);
if (!sdl->window) return false;

// Create renderer
sdl->renderer = SDL_CreateRenderer(sdl->window, -1, 0);
if (!sdl->renderer) return false;

return true;
```

### Error Handling
| Step | Failure Condition | Result |
|------|-------------------|--------|
| SDL_Init | Returns non-zero | `false` |
| SDL_CreateWindow | Returns `NULL` | `false` |
| SDL_CreateRenderer | Returns `NULL` | `false` |

### Notes
- Currently only initializes video subsystem
- Could be extended with `SDL_INIT_AUDIO` for sound support
- Uses default rendering driver (`-1` parameter)

---

## Function: `void sdl_cleanup(sdl_t *sdl)`

### Purpose
Cleans up SDL resources and shuts down SDL subsystems.

### Implementation

```c
if (sdl->window) SDL_DestroyWindow(sdl->window);
if (sdl->renderer) SDL_DestroyRenderer(sdl->renderer);
SDL_Quit();
```

### Resource Management
| Resource | Cleanup Function | Notes |
|----------|-----------------|-------|
| Window | `SDL_DestroyWindow()` | Frees window resources |
| Renderer | `SDL_DestroyRenderer()` | Frees rendering context |
| SDL | `SDL_Quit()` | Shuts down all SDL subsystems |

### Memory Safety
- Checks for `NULL` before destroying resources
- Order: Destroy child resources first (renderer), then parent (window)
- Final: Shutdown SDL entirely

---

## Function: `void sdl_render_frame(sdl_t *sdl, chip8_t *chip8)`

### Purpose
Renders the current CHIP-8 display state to the SDL window.

### Implementation

```c
// Clear screen with black background
SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 255);  // RGBA: Black
SDL_RenderClear(sdl->renderer);                        // Fill entire renderer

// Set pixel color to white
SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 255);  // RGBA: White

// Draw CHIP-8 display pixels
for (int y = 0; y < DISPLAY_HEIGHT; y++) 
{
    for (int x = 0; x < DISPLAY_WIDTH; x++) 
    {
        // Check if pixel is ON (non-zero) in CHIP-8 display buffer
        if (chip8->display[y * DISPLAY_WIDTH + x]) 
        {
            // Create scaled rectangle for pixel
            SDL_Rect pixel = {
                x * SCALE_WINDOW,      // X position (scaled)
                y * SCALE_WINDOW,      // Y position (scaled)
                SCALE_WINDOW,          // Width (20 pixels)
                SCALE_WINDOW           // Height (20 pixels)
            };
            // Fill the rectangle
            SDL_RenderFillRect(sdl->renderer, &pixel);
        }
    }
}

// Update display
SDL_RenderPresent(sdl->renderer);  // Show rendered frame
chip8->draw_flag = false;          // Reset draw flag
```

### Performance Considerations
- Only draws pixels that are ON (non-zero)
- Uses batch rendering with `SDL_RenderPresent()` at the end
- Resets `draw_flag` to avoid unnecessary redraws

---

## Function: `bool sdl_handle_inputs(chip8_t *chip8)`

### Purpose
Processes SDL events (keyboard input, window events) and updates CHIP-8 keypad state.

### Implementation

```c
SDL_Event event;
while (SDL_PollEvent(&event)) 
{
    switch (event.type)
    {
        case SDL_QUIT: return false;  // Exit main loop
            
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            bool pressed = (event.type == SDL_KEYDOWN);
            uint8_t key = 0xFF;  // Invalid key code
            
            // Map SDL keys to CHIP-8 hexadecimal keypad
            switch(event.key.keysym.sym) {
                // Original CHIP-8 keypad layout:
                // 1 2 3 C     -> 1 2 3 C
                // 4 5 6 D     -> Q W E R
                // 7 8 9 E     -> A S D F
                // A 0 B F     -> Z X C V
                
                case SDLK_1: key = 0x1; break;
                case SDLK_2: key = 0x2; break;
                case SDLK_3: key = 0x3; break;
                case SDLK_4: key = 0xC; break;
                case SDLK_q: key = 0x4; break;
                case SDLK_w: key = 0x5; break;
                case SDLK_e: key = 0x6; break;
                case SDLK_r: key = 0xD; break;
                case SDLK_a: key = 0x7; break;
                case SDLK_s: key = 0x8; break;
                case SDLK_d: key = 0x9; break;
                case SDLK_f: key = 0xE; break;
                case SDLK_z: key = 0xA; break;
                case SDLK_x: key = 0x0; break;
                case SDLK_c: key = 0xB; break;
                case SDLK_v: key = 0xF; break;
                case SDLK_ESCAPE: 
                    if (pressed) return false;  // Exit on ESC press
                break;
            }
                
            // Update CHIP-8 keypad state
            if (key < 16) {
                chip8->keypad[key] = pressed;
            }
            break;
        }
    }
}
return true;  // Continue main loop
```

### Event Processing Flow

1. **Poll Events**: `SDL_PollEvent()` gets next event from queue
2. **Event Types**:
   - `SDL_QUIT`: Window close button → return `false`
   - `SDL_KEYDOWN`: Key pressed → set `pressed = true`
   - `SDL_KEYUP`: Key released → set `pressed = false`
3. **Key Mapping**: Convert SDL keycodes to CHIP-8 hex values (0x0-0xF)
4. **State Update**: Set corresponding `chip8->keypad[key]` to pressed state
5. **Special Keys**: ESC key pressed → return `false` (exit)

---
### **Potential Improvements**
```c
// Possible enhancements:
#define FPS 60  // Target frame rate
#define FRAME_DELAY (1000 / FPS)

// Add audio support:
SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

// Add window resize support:
SDL_WINDOW_RESIZABLE flag

// Add error logging:
fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
```

---
