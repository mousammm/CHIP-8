# CHIP-8 Emulator

A CHIP-8 emulator using SDL2 and C.

![CHIP-8 logo](asset/c8-logo.png)

### Prerequisites

- GCC compiler
- SDL2 development libraries

### build
```bash
make
```

## Usage

```bash
./build/chip8 <path-to-rom>
```

### Key Mapping
The CHIP-8 hexadecimal keypad (4×4) is mapped in keyboard:

```
 Keyboard Layout
┌───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ 4 │
├───┼───┼───┼───┤
│ Q │ W │ E │ R │
├───┼───┼───┼───┤
│ A │ S │ D │ F │
├───┼───┼───┼───┤
│ Z │ X │ C │ V │
└───┴───┴───┴───┘
```

- **ESC**: Exit the emulator

### Adding Debug Output
Uncomment or add `printf` statements in `chip8_cycle()` to trace instruction execution.

---
