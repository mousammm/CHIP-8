# case 0xF

* Miscellaneous Operation handles timers memory sound and input 

# case 0xF case 0x07

```c
chip8->V[x] = chip8->delay_timer;
```
* 0xFx07 - LD Vx, DT (Read Delay Timer)

# case 0xF case 0x0A

```c
bool key_pressed = false;
for(int i = 0; i < 16; i++) {
   if(chip8->keypad[i]) {
      chip8->V[x] = i;
      key_pressed = true;
      break;
   }
}
if(!key_pressed) chip8->pc -= 2; // Try again next cycle
```
* 0xFx0A - LD Vx, K (Wait for Key Press)

# case 0xF case 0x15

```c
chip8->delay_timer = chip8->V[x];
```
* 0xFx15 - LD DT, Vx (Set Delay Timer)

# case 0xF case 0x18

```c
chip8->sound_timer = chip8->V[x];
```
* 0xFx18 - LD ST, Vx (Set Sound Timer)

# case 0xF case 0x1E

```c
chip8->I += chip8->V[x];
```

* 0xFx1E - ADD I, Vx (Add to Index Register)

# case 0xF case 0x29

```c
chip8->I = chip8->V[x] * 5; // Each sprite is 5 bytes
```

* 0xFx29 - LD F, Vx (Load Font Character)

# case 0xF case 0x33

```c
chip8->memory[chip8->I] = chip8->V[x] / 100;
chip8->memory[chip8->I + 1] = (chip8->V[x] / 10) % 10;
chip8->memory[chip8->I + 2] = chip8->V[x] % 10;
```

* 0xFx33 - LD B, Vx (BCD Conversion)

# case 0xF case 0x55

```c
for(int i = 0; i <= x; i++) {
   chip8->memory[chip8->I + i] = chip8->V[i];
}
```

* 0xFx55 - LD [I], Vx (Store Registers to Memory)

# case 0xF case 0x65

```c
for(int i = 0; i <= x; i++) {
   chip8->V[i] = chip8->memory[chip8->I + i];
}
```

* 0xFx65 - LD Vx, [I] (Load Registers from Memory)
