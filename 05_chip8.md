# CHIP-8 Emulator Core (`chip8.c`)

## Overview
This file implements the core CHIP-8 emulator logic, including CPU initialization, ROM loading, instruction execution, and timer management.

## Headers

```c
#include "chip8.h"
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h> 
#include <time.h> 
#include <string.h>
```

| Header | Purpose |
|--------|---------|
| `chip8.h` | CHIP-8 structure and function declarations |
| `stdio.h` | Standard I/O for file operations and error messages |
| `stdlib.h` | Standard library functions (`rand()`, `srand()`) |
| `stdint.h` | Fixed-width integer types |
| `time.h` | Time functions for random number seeding |
| `string.h` | Memory operations (`memset()`, `memcpy()`) |

---

## CHIP-8 Fontset

### Font Data Definition

```c
const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
```

### Font Characteristics
- **Total size**: 80 bytes (16 characters × 5 bytes each)
- **Memory location**: Addresses `0x050` to `0x09F` (80-159 decimal)
- **Each character**: 5 bytes (8×5 pixels, 1 byte per row)

### Character Example: '0' (Hexadecimal 0)

| Byte | Binary | Visual Representation |
|------|--------|---------------------|
| `0xF0` | `11110000` | ████···· |
| `0x90` | `10010000` | █··█···· |
| `0x90` | `10010000` | █··█···· |
| `0x90` | `10010000` | █··█···· |
| `0xF0` | `11110000` | ████···· |

---

## Function: `void chip8_init(chip8_t *chip8)`

### Purpose
Initializes the CHIP-8 emulator state to default values.

### Implementation

```c
// Zero-initialize the entire CHIP-8 structure
memset(chip8, 0, sizeof(chip8_t));

// Load fontset into memory (addresses 0x050-0x09F)
memcpy(&chip8->memory[0x050], fontset, sizeof(fontset));

// Set program counter to start of program area
chip8->pc = 0x200;

// Set draw flag to trigger initial screen clear
chip8->draw_flag = true;

// Seed random number generator for RND instruction
srand(time(NULL));
```

### Initialization Steps

1. **Memory Clear**: All registers, memory, and flags set to zero
2. **Font Loading**: Copy font data to addresses `0x050-0x09F`
3. **Program Counter**: Set to `0x200` (standard ROM start address)
4. **Display Flag**: Set to `true` for initial rendering
5. **Random Seed**: Initialize with current time for `RND` instruction

### Memory Layout After Initialization

| Address Range | Content |
|---------------|---------|
| `0x000-0x04F` | Zeroed (reserved for interpreter) |
| `0x050-0x09F` | Font data loaded |
| `0x0A0-0x1FF` | Zeroed (reserved) |
| `0x200-0xFFF` | Zeroed (ready for ROM loading) |

---

## Function: `bool chip8_load_rom(chip8_t *chip8, const char *filename)`

### Purpose
Loads a CHIP-8 ROM file into emulator memory starting at address `0x200`.

### Implementation

```c
// Open ROM file in binary read mode
FILE *file = fopen(filename, "rb");
if (!file) {
    fprintf(stderr, "Cannot open %s file\n", filename);
    return false;
}

// Determine file size
fseek(file, 0, SEEK_END);
long size = ftell(file);
rewind(file);

// Add size validation
if (size > (MEMORY - 0x200)) {
    fprintf(stderr, "ROM too large: %ld bytes\n", size);
    fclose(file);
    return false;
}

// Read ROM data into memory starting at 0x200
if (!fread(&chip8->memory[0x200], 1, size, file)) {
    fprintf(stderr, "Cannot read %s into memory!\n", filename);
    fclose(file);
    return false;
}

fclose(file);
return true;
```

### Error Conditions
1. **File Not Found**: Cannot open specified filename
2. **Read Failure**: Cannot read data from file
3. **Memory Overflow**: ROM size exceeds available memory (should check `size <= 3584`)

### Memory Usage
- **ROM start address**: `0x200` (512 decimal)
- **Maximum ROM size**: 3584 bytes (`0xFFF - 0x200 + 1`)
- **Typical ROM sizes**: 128 bytes to 4KB

---

## Function: `void chip8_timers(chip8_t *chip8)`

### Purpose
Updates the delay and sound timers, which decrement at 60Hz.

### Implementation

```c
// Update delay timer
if (chip8->delay_timer > 0) { 
    chip8->delay_timer--;
}

// Update sound timer
if (chip8->sound_timer > 0) { 
    // Optional: Trigger sound when timer reaches 1
    if (chip8->sound_timer == 1) {
        printf("Beep!\n");
    }
    chip8->sound_timer--;
}
```

### Timer Characteristics

| Timer | Decrement Rate | Purpose | Behavior |
|-------|---------------|---------|----------|
| **Delay Timer** | 60Hz | Game timing | Decrements while non-zero |
| **Sound Timer** | 60Hz | Audio output | Decrements while non-zero, beeps while >0 |

### Timer Usage in CHIP-8 Programs
- **Delay Timer**: Used for timing game events, animations, and delays
- **Sound Timer**: Produces beeping sound when >0, typically for game sounds

### Implementation Notes

1. **60Hz Rate**: Should be called 60 times per second
2. **Sound Output**: Current implementation uses console output
3. **SDL Integration**: Could integrate with SDL audio for actual sound

---

## Memory Map After Complete Initialization

| Address Range | Content | Size |
|---------------|---------|------|
| `0x000-0x04F` | Zeroed | 80 bytes |
| `0x050-0x09F` | Font Data | 80 bytes |
| `0x0A0-0x1FF` | Zeroed | 352 bytes |
| `0x200-0x200+size` | ROM Data | ROM size |
| `0x200+size-0xFFF` | Zeroed | Remaining memory |

---
