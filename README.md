# 🫖 Neon Teapot

![C++](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)
![Platform](https://img.shields.io/badge/platform-Termux%20%7C%20Android-green?logo=android)
![Version](https://img.shields.io/badge/version-1.0.0-orange)
![License](https://img.shields.io/badge/license-MIT-yellow)

Just a little terminal project I made in C++.

It draws a rotating 3D-style teapot using text characters and ANSI colors. Nothing too serious, I just wanted to see how cool I could make terminal graphics look.

## What it does

- Rotating teapot animation
- Colorful terminal output
- FPS counter
- Runs in Termux on Android

## Running it

You'll need Termux and clang.

```bash
pkg update
pkg install clang
```

Compile the C++ file:

```bash
clang++ -std=c++17 -O2 teapot.cpp -o teapot
```

Run it:

```bash
./teapot
```

Press `Ctrl+C` to stop.

## Files

- `teapot.cpp` — C++ version
- `teapot.cs` — C# version

## Notes

This is just a fun little experiment, so don't expect a real 3D engine. It renders everything directly in the terminal.

If you try it out, feel free to mess with the code and make it your own.

Have fun!
