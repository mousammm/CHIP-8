# case 0x8 // 0x8xxx - Arithmetic and Logic Operations

# 0x8 case 0x0

```c
chip8->V[x] = chip8->V[y];
```

* 8xy0 - LD Vx, Vy

# 0x8 case 0x1

```c
chip8->V[x] |= chip8->V[y];
```

* 8xy1 - OR Vx, Vy

# 0x8 case 0x2

```c
chip8->V[x] &= chip8->V[y];
```
* 8xy2 - AND Vx, Vy

# 0x8 case 0x3

```c
chip8->V[x] ^= chip8->V[y];
```

* 8xy3 - XOR Vx, Vy

# 0x8 case 0x4

```c
uint16_t sum = chip8->V[x] + chip8->V[y];
chip8->V[x] = sum & 0xFF;
chip8->V[0xF] = (sum > 0xFF);
```

* 8xy4 - ADD Vx, Vy (with carry)
* V1 + V2 = 0x80 + 0x90 = 0x110 (272 in decimal)
* Result:
* V1 = 0x10  // Only keep lower 8 bits
* VF = 1     // Carry flag set because 0x110 > 0xFF

# 0x8 case 0x5

```c
chip8->V[0xF] = (chip8->V[x] > chip8->V[y]);
chip8->V[x] -= chip8->V[y];
```

* 8xy5 - SUB Vx, Vy 
* 0x90 - 0x80 = 0x10
* Result:
* V1 = 0x10
* VF = 1     // No borrow needed (V1 > V2) */

# 0x8 case 0x6

```c
chip8->V[0xF] = chip8->V[x] &  0x1; // set carry flag to least significant bit 
chip8->V[x] >>= 1;                  // shift right by 1
```

* 8xy6 - SHR Vx, Vy 
* Shift V1 right: 1000 0101 → 0100 0010 (0x42)
* Least significant bit was 1
* Result:
* V1 = 0x42
* VF = 1     // Carry = bit that was shifted out  */

# 0x8 case 0x7

```c
chip8->V[0xF] = (chip8->V[y] > chip8->V[x]);
chip8->V[x] = chip8->V[y] - chip8->V[x];
```

* 8xy7 - SUB Vx, Vy 
* V2 - V1 = 0x30 - 0x20 = 0x10
* Result:
* V1 = 0x10  // Result stored in V1
* VF = 1     // No borrow (V2 > V1) */

# 0x8 case 0xE

```c
chip8->V[0xF] = (chip8->V[x] &  0x80) >> 7; // set carry flag to most significant bit 
chip8->V[x] <<= 1;                          // shift left by 1
```

* 8xyE - SHL Vx, Vy
* Shift V1 left: 1100 0001 → 1000 0010 (0x82)
* Most significant bit was 1
* Result:
* V1 = 0x82
* VF = 1     // Carry = bit that was shifted out */
