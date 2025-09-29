# Loading rom into memeory 

open the ~/chip8/src/chip8/chip8.c  (chip8_load_rom) function

Purpose

The function loads a CHIP-8 ROM file into the emulator's memory starting at address 0x200.

1. File Opening

```c
FILE *file = fopen(filename, "rb");
```

    Opens the file in binary read mode ("rb")
    Returns false if the file cannot be opened

2. File Size Check

```c
fseek(file,0,SEEK_END);
long size = ftell(file);
rewind(file);
```

    fseek moves to the end of the file
    ftell gets the current position (which is the file size)
    rewind returns to the beginning of the file

3. Size Validation

```c

if(size > MEMORY - 0x200)
```

    CHIP-8 memory is typically 4096 bytes (MEMORY = 4096)
    The ROM loads at address 0x200 (512 in decimal)
    This leaves 4096 - 512 = 3584 bytes for the ROM
    Ensures the ROM fits in available memory

4. Memory Loading

```c

fread(&chip8->memory[0x200],1,size,file)
```

    Reads the file content directly into memory starting at address 0x200
    Uses the actual file size determined earlier

Return Values

    Returns true on successful load
    Returns false on any error (file not found, too large, read error)
