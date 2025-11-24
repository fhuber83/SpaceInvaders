# Space Invaders - Build Instructions

## Linux

### Prerequisites
```bash
sudo apt install build-essential cmake libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
```

### Build with CMake
```bash
mkdir build
cd build
cmake ..
cmake --build .
./space_invaders
```

### Build with Make (original)
```bash
make
./space_invaders
```

## Windows

### Option 1: Visual Studio with vcpkg

1. Install Visual Studio 2022 (Community Edition)
2. Install vcpkg:
   ```cmd
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   bootstrap-vcpkg.bat
   vcpkg integrate install
   ```

3. Install SDL2 libraries:
   ```cmd
   vcpkg install sdl2:x64-windows sdl2-ttf:x64-windows sdl2-image:x64-windows
   ```

4. Build with CMake:
   ```cmd
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
   cmake --build .
   ```

### Option 2: MSYS2/MinGW

1. Install MSYS2 from https://www.msys2.org/
2. Open MSYS2 MinGW64 terminal:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_image
   ```

3. Build:
   ```bash
   mkdir build
   cd build
   cmake .. -G "MinGW Makefiles"
   cmake --build .
   ```

## macOS

### Prerequisites
```bash
brew install cmake sdl2 sdl2_ttf sdl2_image
```

### Build
```bash
mkdir build
cd build
cmake ..
cmake --build .
./space_invaders
```

## Game Controls

- **A/D**: Move left/right
- **Space**: Shoot
- **ALT+Enter**: Toggle fullscreen
- **Enter**: Start new game (on game over/win screen)

## Notes

- Make sure all image files (background.png, boss_background.jpg, trophy.png, gorilla.png, barrel.png) are in the same directory as the executable
- The game uses the DejaVu Sans Bold font for text rendering
