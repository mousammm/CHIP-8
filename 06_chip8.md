# CHIP-8 Instruction Cycle: Fetch, Decode, Execute

## Overview
The `chip8_cycle()` function implements the core CPU cycle of the CHIP-8 emulator. It fetches, decodes, and executes one instruction per cycle.

## 1. Instruction Fetch

### Opcode Assembly
```c
uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
chip8->pc += 2;
```

### Step-by-Step Breakdown

**Example**: Memory contains `0x12` at address `chip8->pc` and `0x34` at `chip8->pc + 1`

1. **Fetch high byte**: `chip8->memory[chip8->pc] = 0x12`
2. **Shift left 8 bits**: `0x12 << 8 = 0x1200` (moves to high byte position)
3. **Fetch low byte**: `chip8->memory[chip8->pc + 1] = 0x34`
4. **Combine with OR**: `0x1200 | 0x34 = 0x1234`

**Result**: Complete 16-bit opcode `0x1234`

### Program Counter Update
- Increment `pc` by 2 to point to the next instruction
- CHIP-8 instructions are always 2 bytes (16 bits)

### Big-Endian Architecture
- **High byte first**: `memory[pc]` = most significant byte (MSB)
- **Low byte second**: `memory[pc + 1]` = least significant byte (LSB)
- This shift-and-OR operation preserves CHIP-8's big-endian format

### Original Address Storage
```c
uint16_t pc_before = chip8->pc - 2;  // Address where this opcode was fetched
```
- Useful for debugging and certain instructions (like jump with offset)
- Records the exact memory location of the current instruction

## 2. Opcode Decoding with Bit Masking

### Bit Masking Concept
Bit masking extracts specific bits from a binary value using **AND operations**:

**Basic Rule**: 
- `1 AND 1 = 1`
- `1 AND 0 = 0`
- `0 AND anything = 0`

## Mask Application Examples

### Example 1: Extracting `nnn` (12-bit address)

```
Opcode:    1101 0010 1011 1100  (0xD2BC)
Mask:      0000 1111 1111 1111  (0x0FFF)
Result:    0000 0010 1011 1100  (0x02BC = nnn)
```

**What happened:**
- Bits 12-15 (first nibble) were masked out
- Bits 0-11 were preserved
- Result: `0x02BC` = the address `nnn`

### Example 2: Extracting `x` (register identifier)

```
Opcode:    1101 0010 1011 1100  (0xD2BC)
Mask:      0000 1111 0000 0000  (0x0F00)
Result:    0000 0010 0000 0000  (0x0200)
Shift >>8: 0000 0000 0000 0010  (0x02 = x)
```

**What happened:**
- Bits 8-11 were preserved, all others zeroed
- Result: `0x0200` (bits 8-11 in positions 8-11)
- Right shift by 8: `0x02` = register V2

## Opcode Component Extraction in Code

```c
uint8_t first_nibble = (opcode & 0xF000) >> 12;  // 1111 0000 0000 0000  // Bits 12-15
uint8_t x = (opcode & 0x0F00) >> 8;              // 0000 1111 0000 0000  // Bits 8-11
uint8_t y = (opcode & 0x00F0) >> 4;              // 0000 0000 1111 0000  // Bits 4-7
uint8_t n = opcode & 0x000F;                     // 0000 0000 0000 1111  // Bits 0-3
uint8_t kk = opcode & 0x00FF;                    // 0000 0000 1111 1111  // Bits 0-7
uint16_t nnn = opcode & 0x0FFF;                  // 0000 1111 1111 1111  // Bits 0-11
```
## Complete Example: Opcode `0x1234`

**Binary representation**: `0001 0010 0011 0100` (16 bits)

### Step-by-step extraction:

| Component | Mask | Operation | Result | Value |
|-----------|------|-----------|--------|-------|
| **Original** | - | - | `0001 0010 0011 0100` | `0x1234` |
| **first_nibble** | `0xF000` | `& mask, >> 12` | `0001` | `0x1` |
| **x** | `0x0F00` | `& mask, >> 8` | `0010` | `0x2` |
| **y** | `0x00F0` | `& mask, >> 4` | `0011` | `0x3` |
| **n** | `0x000F` | `& mask` | `0100` | `0x4` |
| **kk** | `0x00FF` | `& mask` | `0011 0100` | `0x34` |
| **nnn** | `0x0FFF` | `& mask` | `0010 0011 0100` | `0x234` |

## Bit Mask Values Explained

| Mask | Binary | Extracts | Purpose |
|------|--------|----------|---------|
| `0xF000` | `1111 0000 0000 0000` | Bits 12-15 | Instruction category |
| `0x0F00` | `0000 1111 0000 0000` | Bits 8-11 | Register identifier (Vx) |
| `0x00F0` | `0000 0000 1111 0000` | Bits 4-7 | Register identifier (Vy) |
| `0x000F` | `0000 0000 0000 1111` | Bits 0-3 | 4-bit literal or operation code |
| `0x00FF` | `0000 0000 1111 1111` | Bits 0-7 | 8-bit literal value |
| `0x0FFF` | `0000 1111 1111 1111` | Bits 0-11 | 12-bit memory address |

## Why This Works

1. **AND operation (`&`)**: Preserves only bits where both operands have 1
   - `1 & 1 = 1`
   - `0 & anything = 0`

2. **Right shift (`>>`)**: Moves bits to lower positions for easier use
   - Shifting by 8 moves bits 8-11 to positions 0-3
   - Shifting by 12 moves bits 12-15 to positions 0-3

3. **No shift for low bits**: Bits 0-3, 0-7, and 0-11 are already in correct positions for their intended use

## Practical Usage in Instructions

Once extracted, these components are used to determine which instruction to execute:

- **`first_nibble`**: Primary instruction category (0-F)
- **`x`, `y`**: Register identifiers (0x0-0xF = V0-VF)
- **`n`**: Sub-operation within category 8, or sprite height in draw instruction
- **`kk`**: 8-bit immediate value (0x00-0xFF)
- **`nnn`**: 12-bit memory address (0x000-0xFFF)

This bit masking approach efficiently extracts all necessary information from a 16-bit opcode in just 6 operations, enabling the emulator to quickly determine which instruction to execute.
