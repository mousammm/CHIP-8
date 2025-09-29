# Chip8 timer function

Purpose

Manages the CHIP-8's countdown timers that run at 60Hz (decrement 60 times per second).

The Two Timers

1. Delay Timer

```c

if (chip8->delay_timer > 0) { 
   chip8->delay_timer--;
}
```

    Purpose: Used for general timing in games (controlling game speed, animations, etc.)
    Behavior: Simply counts down from a value to 0 at 60Hz
    Usage: Programs can read this timer value to create delays

2. Sound Timer

```c

if (chip8->sound_timer > 0) { 
   if (chip8->sound_timer == 1) { 
      printf("kela!\n");
   }
   chip8->sound_timer--;
}
```
    Purpose: Controls sound playback
    Special Behavior:
        When it reaches 1, it triggers a sound (here printing "kela!")
        This indicates the sound should play while the timer is > 0
        The sound stops when the timer reaches 0

This timing system is crucial for CHIP-8 games to maintain proper speed and audio synchronization
