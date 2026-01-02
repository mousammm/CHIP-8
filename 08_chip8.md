# from 0x0 to 0x7

# case 0x0

```c
if (kk == 0xE0) { // Clear Screen 
   memset(chip8->display, 0, sizeof(chip8->display));
   chip8->draw_flag = true;
   break;
} else if (kk == 0xEE) { // Return from subroutine
   chip8->sp--;
   chip8->pc = chip8->stack[chip8->sp];
   break;
} else {
    printf("SYS 0x%03X (ignore)", nnn); // Backward compatiblity
}
```

# case 0x1

```c
chip8->pc = nnn;
```

* opcode = 0x1234
* first_nibble = 0x1
* nnn = 0x234  // Jump target address
* chip8->pc = 0x234;  // Program continues from address 0x234

# case 0x2

```c
chip8->stack[chip8->sp] = chip8->pc; //save return address
chip8->sp++;                         // move stack pointer up
chip8->pc = nnn;                     // jump to subroutines
```

* Calls a subroutine at address nnn
* Saves return address on the stack before jumping
* Like a function call in high-level languages

# case 0x3

 ```c
 if(chip8->V[x] == kk) chip8->pc += 2;
 ```

* Compares register Vx with immediate value kk
* Skips next instruction if they are equal
* Conditional skip - doesn't always execute

# case 0x4

```c
if(chip8->V[x] != kk) chip8->pc += 2;
```

* Skip if Vx == kk

# case 0x5

```c
if(chip8->V[x] != kk) chip8->pc += 2;
```

* Skip if Vx != kk (opposite of 0x3)

# case 0x6

```c
chip8->V[x] = kk;
```

* LD Vx, byte (Load immediate value into register)

# case 0x7

```c
chip8->V[x] += kk;
```

* Add kk to Vx
* opcode = 0x7105
* first_nibble = 0x7
* x = 0x1      // Register V1
* kk = 0x05    // Value to add
*              
* // Execution:
* chip8->V[1] += 0x05;  // Simple addition (no carry flag)
