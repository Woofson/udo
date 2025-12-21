# µDo!

A minimalist, command-driven web browser by Tintin (github.com/Woofson/udo).

## Description

µDo! is a lightweight, frameless web browser designed for power users. All interactions are handled through a command bar, keyboard shortcuts, or a simple right-click menu. The browser is highly configurable via a `config.json` file.

## Build Dependencies

To build µDo!, you will need the following dependencies:
- A C++20 compliant compiler (e.g., g++)
- CMake (version 3.16 or higher)
- Qt 6 (version 6.2 or higher)
  - Core
  - Gui
  - Widgets
  - WebEngineCore
  - WebEngineWidgets

On a Debian-based Linux distribution (like Ubuntu), you can install these with:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake qt6-base-dev qt6-webengine-dev
```

## Build Instructions

1. Clone the repository.
2. Navigate to the project directory.
3. Create a build directory and move into it:
   ```bash
   mkdir build && cd build
   ```
4. Run CMake to configure the project:
   ```bash
   cmake ..
   ```
5. Compile the project:
   ```bash
   make
   ```

## Running the Browser

After a successful build, the executable will be located in the `build` directory.

```bash
./build/udo
```

On the first run, the browser will create a configuration directory at `~/.config/udo/` and copy the default `config.json` there. You can customize the browser's behavior by editing this file.