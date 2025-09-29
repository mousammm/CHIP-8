## ⚙️ **CHIP-8 REGISTERS**

### 🔹 **General Purpose Registers (16 x 8-bit)**
- **V0 to VF** - 16 general purpose registers
- **VF is special** - carries the *carry flag* and is used for *collision detection* in games
- Each register stores *8-bit values* (0-255)

### 🔹 **Index Register (16-bit)**
- **Equivalent to a pointer in C/C++**
- Holds *memory addresses* as its value
- Used for operations involving memory locations

### 🔹 **Program Counter (16-bit)**
- **Special register** that stores the *memory address* of the **next instruction** to be executed
- Automatically increments after each instruction fetch

---

## 🗂️ **STACK ARCHITECTURE**

### 🔹 **Stack Overview**
A **Last-In-First-Out (LIFO)** data structure similar to a *stack of plates*:
- **Push** - add item to top
- **Pop** - remove item from top

### 🔹 **Stack Pointer (8-bit)**
- **Special register** that holds the *memory address* of the **top of the stack**
- Manages subroutine calls and returns

---

## ⏰ **TIMERS**

### 🔹 **Delay Timer**
- **Decrements at 60Hz** when non-zero
- Used for *timing operations* in games

### 🔹 **Sound Timer** 
- **Decrements at 60Hz** when non-zero
- **Produces beeping sound** while non-zero

---

## 🖥️ **DISPLAY SYSTEM**

### 🔹 **Display Specifications**
- **64 × 32 pixel** monochrome display
- **2048 total pixels** (64 × 32)
- Each pixel is either *ON (1)* or *OFF (0)*
- Uses *XOR* drawing for collision detection

---

## 🎮 **INPUT SYSTEM**

### 🔹 **Hexadecimal Keypad**
- **16-key input** numbered from `0` to `F`

