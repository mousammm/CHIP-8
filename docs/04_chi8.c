# CHIP-8 Architecture: Memory & Instruction Design

## 1. Memory Structure

**CHIP-8 has 4KB of memory/RAM** - a fundamental component of the system.

### Memory as an Array
Think of memory as **an array of 4096 elements**, where each element can store:

- **1 byte (8 bits)** of data
- **Binary range**: `00000000` to `11111111`
- **Hexadecimal range**: `0x00` to `0xFF`
- **Decimal range**: 0 to 255 values

## Why Memory Structure Matters: OPCODES

### The Critical Connection
**CHIP-8 instructions (OPCODES) are made of 2 bytes**.

This means:
- **2 elements** from the memory array hold **a single CHIP-8 instruction**
- Each instruction consists of **2 bytes**
- Hexadecimal range for each byte: `0x00` to `0xFF`

## CRITICAL DISTINCTION

**Important:** Do NOT confuse the *value* a single element holds with the *address* of the element!

**Example:**
- Element at **address `0x111`** contains value `0xF0`
- Element at **address `0x112`** contains value `0x29`
- **Together** they form opcode: `0xF029`

## Memory Map

| Hex Address | Decimal Range | Description |
|-------------|---------------|-------------|
| `0x000-0x045` | 0-79 | Original CHIP-8 Interpreter |
| `0x050-0x0A0` | 80-160 | Font Data (16×5 bytes) |
| `0x0A1-0x1FF` | 161-511 | Reserved (unused) |
| `0x200-0xFFF` | 512-4095 | Program Area (3.5KB) |

## Font System

- **Each character**: 1 byte (8 bits) × 5 = 5 bytes (40 bits)
- **Total characters**: 16 (0-F in hexadecimal)
- **Total memory used**: 16 characters × 5 bytes = 80 bytes
- **Memory location**: `0x050` to `0x09F`

### Memory Layout
Think of the font set as a **continuous block of memory** - essentially an array.

### Character '0' Representation
Here's how the character '0' is represented in memory:

| Index | Hex Value | Binary Value | Visual Pattern |
|-------|-----------|--------------|----------------|
| **0** | `0xF0` | `11110000` | `■■■■....` |
| **1** | `0x90` | `10010000` | `■..■....` |
| **2** | `0x90` | `10010000` | `■..■....` |
| **3** | `0x90` | `10010000` | `■..■....` |
| **4** | `0xF0` | `11110000` | `■■■■....` |

**Result**: This forms the **character '0'** when displayed!

## How Font Rendering Works

### Bit-to-Pixel Mapping
- **1 (ON)** → Pixel is **ILLUMINATED** (white/square)
- **0 (OFF)** → Pixel is **DARK** (black/empty)

## CHIP-8 Registers

### General Purpose Registers (16 × 8-bit)
- **V0 to VF** - 16 general purpose registers
- **VF is special** - carries the *carry flag* and is used for *collision detection* in games
- Each register stores 8-bit values (0-255)

### Index Register (16-bit)
- **Equivalent to a pointer in C/C++**
- Holds *memory addresses* as its value
- Used for operations involving memory locations

### Program Counter (16-bit)
- **Special register** that stores the *memory address* of the **next instruction** to be executed
- Automatically increments after each instruction fetch

## Stack System

### Stack Overview
A **Last-In-First-Out (LIFO)** data structure similar to a *stack of plates*:
- **Push** - add item to top
- **Pop** - remove item from top

### Stack Pointer (8-bit)
- **Special register** that holds the *memory address* of the **top of the stack**
- Manages subroutine calls and returns

## Timers

### Delay Timer
- **Decrements at 60Hz** when non-zero
- Used for *timing operations* in games

### Sound Timer
- **Decrements at 60Hz** when non-zero
- **Produces beeping sound** while non-zero

## Display System

### Display Specifications
- **64 × 32 pixel** monochrome display
- **2048 total pixels** (64 × 32)
- Each pixel is either ON (1) or OFF (0)

## Input System

### Hexadecimal Keypad
- **16-key input** numbered from `0` to `F`
- Organized in a 4×4 grid configuration
