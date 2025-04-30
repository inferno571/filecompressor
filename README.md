# File Compressor (Huffman Coding Based)

This is a simple terminal-based file compressor and decompressor built using Huffman coding in C++. It features a stylized ASCII art UI and there are seperate versions for both **Windows** and **Linux** platforms.

---
##  Features

- Compress and decompress text files using Huffman encoding
- Sleek terminal interface with animated progress bar
- Minimal dependencies — fully written in standard C++

---

##  Demo
## Windows:- 
![{3BEEF3B1-2307-4440-8A66-795EBB306A07}](https://github.com/user-attachments/assets/dec0caf6-3e84-45cd-a813-b1e9bc53b577)
## Linux:-
(TBA)

## Key Differences:-

| Feature                  | Windows Version                             | Linux Version                              |
|--------------------------|----------------------------------------------|---------------------------------------------|
| **Terminal Encoding**    | May require `chcp 65001` for UTF-8 support   | Usually supports UTF-8 out of the box       |
| **Clear Screen Command** | `system("cls")`                              | `system("clear")`                           |
| **File Paths**           | Use `\\` or raw strings (`C:\\path\\to\\file`) | Use POSIX-style paths (`/home/user/file`)  |
