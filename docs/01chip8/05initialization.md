# Initialization the chip8 

Code for initializating chip8 

```c
void chip8_init(chip8_t *chip8){
   memset(chip8,0,sizeof(chip8_t));

   // load font 
   memcpy(&chip8->memory[0],fontset,sizeof(fontset));

   // set program counter 
   chip8->pc = 0x200;
   chip8->draw_flag = true;

   // Seed random number generator
   srand(time(NULL));
}
```

Lets understand what's hapenning here

1. memset(chip8,0,sizeof(chip8_t));
    Purpose: Clears the entire CHIP-8 structure to zero
    What it does: Sets all memory, registers, timers, display, etc. to 0
    Equivalent to: Starting with a clean slate

2. memcpy(&chip8->memory[0],fontset,sizeof(fontset));
    Purpose: Loads the CHIP-8 fontset into memory
    Location: Copies the font data to memory addresses 0x000 to 0x04F
    Why: CHIP-8 has a built-in font for hexadecimal digits (0-F) that programs use to display text
    Font size: 80 bytes (16 characters × 5 bytes each)

3. chip8->pc = 0x200;
    Purpose: Sets the program counter to the correct starting address
    Why 0x200: CHIP-8 programs traditionally start at memory address 0x200
        Addresses 0x000-0x1FF are reserved for the interpreter
        0x000-0x04F: Font data (which we just loaded)
        0x050-0x1FF: Unused/available for interpreter use

4. chip8->draw_flag = true;
    Purpose: Signals that the display needs to be drawn initially
    How it works: The emulator checks this flag to know when to update the screen
    Optimization: Avoids redrawing the screen every cycle if nothing changed

5. srand(time(NULL));
    Purpose: Seeds the random number generator
    Why: CHIP-8 has an instruction (Cxkk) that generates random numbers
    Using time(NULL): Ensures different random sequences each time the emulator runs

What Gets Initialized to Zero:
    Memory (except font area): All 4096 bytes cleared
    V registers (V0-VF): All 16 general-purpose registers set to 0
    I register: Index register set to 0
    Stack: Call stack and stack pointer (sp) cleared
    Timers: Delay and sound timers set to 0
    Display: All 64×32 pixels turned off
    Keypad: All keys marked as not pressed

This initialization puts the CHIP-8 in the exact state expected by CHIP-8 programs, ready to load and execute ROMs starting from the traditional entry point at 0x200.















