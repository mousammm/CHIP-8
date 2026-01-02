# from 0x9 to 0xE

# case 0x9

```c
if(chip8->V[x] != chip8->V[y]) chip8->pc += 2;
```

* 9xy0 - SNE Vx, Vy Skip if Vx != Vy

# case 0xA

```c
chip8->I = nnn;
```

* Annn - LD I, addr (Load Index Register with address)
* Loads a 12-bit address into the index register I
* A = opcode identifier (0xA)
* nnn = 12-bit address to load into I */

# case 0xB

```c
chip8->pc = nnn + chip8->V[0];
```

* Bnnn - JP V0, addr (Jump to address V0 + nnn)

# case 0xC

```c
chip8->V[x] = (rand() % 256) & kk;
```
* Cxkk - RND Vx, byte (Random number generation)
*Where:
 - C = opcode identifier (0xC)
 - x = register index (0-F)
 - kk = 8-bit mask value */

# case 0xD

```c
uint8_t x_pos = chip8->V[x] % DISPLAY_WIDTH; // X cordinate (wrapped) x_pos = 70 % 64 = 6
uint8_t y_pos = chip8->V[y] % DISPLAY_HEIGHT;// Y cordinate (wrapped) y_pos = 40 % 32 = 8
chip8->V[0xF] = 0; // Reset colision flag 
   
for(int row = 0; row < n; row++) { // process each sprite row 
       //reads the sprite data from memory, one row at a time
      uint8_t sprite_byte = chip8->memory[chip8->I + row];
      
      for(int col = 0; col < 8; col++) {  // process each pixel in row
          // Bit Extraction: (0x80 >> col) scans bits from left to right (MSB first)
          /* sprite_byte = 0xA5 (1010 0101 binary)
             col=0: 0x80 >> 0 = 1000 0000 → checks bit 7 (1)  DRAW
             col=1: 0x80 >> 1 = 0100 0000 → checks bit 6 (0)  skip  
             col=2: 0x80 >> 2 = 0010 0000 → checks bit 5 (1)  DRAW */
         if((sprite_byte & (0x80 >> col)) != 0) { 
             // calculate the exact screen position for each sprite pixel
            int pixel_x = (x_pos + col) % DISPLAY_WIDTH;
            int pixel_y = (y_pos + row) % DISPLAY_HEIGHT;
             // converts (X,Y) coordinates into a 1D array index
            int pixel_index = pixel_y * DISPLAY_WIDTH + pixel_x;
            
            if(chip8->display[pixel_index] == 1) {
               chip8->V[0xF] = 1; // Collision detection
            }
            chip8->display[pixel_index] ^= 1;
         }
      }
   }
   chip8->draw_flag = true;
```

* Dxyn - Draw sprite at (Vx, Vy) with n bytes of sprite data
* D = opcode identifier (0xD)
  - x = X coordinate register index
  - y = Y coordinate register index  
  - n = sprite height in pixels (1-15) */

# case 0xE: // Keyboard Input Instructions

 ```c
 switch(kk) {
    case 0x9E: // Ex9E - Skip if key in Vx is pressed
       if(chip8->keypad[chip8->V[x]]) chip8->pc += 2;
       break;
    case 0xA1: // ExA1 - Skip if key in Vx is NOT pressed
       if(!chip8->keypad[chip8->V[x]]) chip8->pc += 2;
       break;
 }
 ```
