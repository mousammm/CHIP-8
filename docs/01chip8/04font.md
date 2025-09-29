# CHIP-8 FONT SET ARCHITECTURE

## 🎨 **Font Set Structure**

### 🔢 **Basic Specifications**
- **Each character**: `1 byte (8 bits) × 5 = 5 bytes (40 bits)`
- **Total characters**: `16` (0-F in hexadecimal)
- **Total memory used**: `16 characters × 5 bytes = 80 bytes`
- **Memory location**: `0x050 to 0x09F`

### 🗂️ **Memory Layout**
Think of the font set as a **continuous block of memory** - essentially an **array from index 0 to 79**:

---

## 🔍 **Character '0' - Detailed Breakdown**

### 🖼️ **Visual Representation**

### 📊 **Byte-by-Byte Representation**
| Index | Hex Value | Binary Value | Visual Pattern |
|-------|-----------|--------------|----------------|
| **0** | `0xF0` | `11110000` | `■■■■░░░░` |
| **1** | `0x90` | `10010000` | `■░░■░░░░` |
| **2** | `0x90` | `10010000` | `■░░■░░░░` |
| **3** | `0x90` | `10010000` | `■░░■░░░░` |
| **4** | `0xF0` | `11110000` | `■■■■░░░░` |



**Result**: This forms the **character '0'** when displayed!

---

## 🎯 **How Font Rendering Works**

### 🔄 **Bit-to-Pixel Mapping**
- **1 (ON)** → Pixel is **ILLUMINATED** (white/square)
- **0 (OFF)** → Pixel is **DARK** (black/empty)
