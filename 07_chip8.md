 # implement of opcode bird eye view

 ```c
 switch (first_nibble) 
 {
      case 0x0:
      case 0x1: // 1nnn jump to address
      case 0x2: // 2nnn - Call subroutine at nnn
      case 0x3: // 3xkk - Skip if Vx == kk
      case 0x4: // 4xkk - Skip if Vx != kk (opposite of 0x3)
      case 0x5: // 5xy0 - Skip if Vx == Vy
      case 0x6: // 6xkk - LD Vx, byte (Load immediate value into register)
      case 0x7: // 7xkk - Add kk to Vx
      case 0x8: // 0x8xxx - Arithmetic and Logic Operations
         switch (n)
         {
            case 0x1: // 8xy1 - OR Vx, Vy
            case 0x2: // 8xy2 - AND Vx, Vy
            case 0x3: // 8xy3 - XOR Vx, Vy
            case 0x4: // 8xy4 - ADD Vx, Vy (with carry)
            case 0x5: // 8xy5 - SUB Vx, Vy 
            case 0x6: // 8xy6 - SHR Vx, Vy 
            case 0x7: // 8xy7 - SUB Vx, Vy 
            case 0xE: // 8xyE - SHL Vx, Vy
         } 

      case 0x9: // 9xy0 - SNE Vx, Vy Skip if Vx != Vy
      case 0xA: // Annn - LD I, addr (Load Index Register with address)
      case 0xB: // Bnnn - JP V0, addr (Jump to address V0 + nnn)
      case 0xC: // Cxkk - RND Vx, byte (Random number generation)
      case 0xD: // Dxyn - Draw sprite at (Vx, Vy) with n bytes of sprite data
      case 0xE: // Keyboard Input Instructions
      case 0xF: // Miscellaneous Operation handles timers memory sound and input 
         switch (kk)
         {
            case 0x07: // 0xFx07 - LD Vx, DT (Read Delay Timer)
            case 0x0A: // 0xFx0A - LD Vx, K (Wait for Key Press)
            case 0x15: // 0xFx15 - LD DT, Vx (Set Delay Timer)
            case 0x18: // 0xFx18 - LD ST, Vx (Set Sound Timer)
            case 0x1E: // 0xFx1E - ADD I, Vx (Add to Index Register)
            case 0x29: // 0xFx29 - LD F, Vx (Load Font Character)
            case 0x33: // 0xFx33 - LD B, Vx (BCD Conversion)
            case 0x55: // 0xFx55 - LD [I], Vx (Store Registers to Memory)
            case 0x65: // 0xFx65 - LD Vx, [I] (Load Registers from Memory)
            break;
         } 
      break; 

      default:
         printf("Unknown opcode: 0x%04X\n", opcode);
      break;
 
   }
}
```
