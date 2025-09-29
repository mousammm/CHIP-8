# CHIP-8 ARCHITECTURE: Memory & Instruction Design

## 🧠 **1. Memory Structure**

**CHIP-8 has *4KB* of memory/RAM** - let's visualize this fundamental component.

### 🔢 **Memory as an Array**
Think of the memory as **an array of 4096 elements**, where *each element* can hold up to:

- **1 byte (8 bits)** of data
- **Binary range**: `00000000` to `11111111`  
- **Hexadecimal range**: `0x00` to `0xFF`
- **Decimal range**: *0 to 255* values

> **Key Point**: A single memory element can store *up to 255 different values* in decimal.

---

## ⚡ **Why Memory Structure Matters: OPCODES**

### 🎯 **The Critical Connection**
**CHIP-8 instructions (OPCODES) are made of *2 bytes***. 

This means:
- **2 elements** from the memory array hold **a single CHIP-8 instruction**
- **Total opcode range**: `0x0000` to `0xFFFF`
- **Decimal possibilities**: *65,535 different values*
- **Binary span**: 16 bits per instruction

---

## ⚠️ **CRITICAL DISTINCTION**

> 🚨 **[[!NOTE]]**
> **Do NOT confuse the *value* a single element holds with the *address* of the element!**
> 
> **Example:**
> - An element at **address `0x111`** 
> - Can contain a **value `0xF0`**
> - The *next element* at **address `0x112`**
> - Can contain a **value `0x29`**
> - **Together** they form opcode: `0xF029`

Visual Block 

| hex | decimal | description |
| --------------- | --------------- | --------------- |
| 0x000-0x045 | 0-79 | original CHIP-8 Intrepreter |
| 0x050-0x0A0 | 80-160 | font load (16x5bytes)|
| 0x0A1-0x1FF | 161-511 | Reserved (unused) |
| 0x200-0xFFF | 512-4095 | Program area(3.5kb)|



---

## 🎯 **Why This Knowledge is Essential**

We need this understanding to:
- **Decode instructions** properly from memory
- **Execute programs** correctly by reading consecutive bytes
- **Manage program flow** through memory addresses
- **Implement accurate CHIP-8 emulators**

---
