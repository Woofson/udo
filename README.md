# µDo! v2.0.3

A minimalist, command-driven web browser by Tintin (github.com/Woofson/udo).

## Description

µDo! is a lightweight, frameless web browser designed for power users. All interactions are handled through a command bar, keyboard shortcuts, or a simple right-click menu. The browser is highly configurable via a `config.json` file and features a fully modular theme engine.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Installation

### Automated Script (Linux Generic)
For convenience, helper scripts are provided to build and install the browser system-wide.

**Install:**
```bash
sudo ./scripts/install.sh
```

**Uninstall:**
```bash
sudo ./scripts/uninstall.sh
```

### Manual Build
If you prefer to build manually:

1. **Install Dependencies:**

   *   **Debian/Ubuntu:**
       ```bash
       sudo apt-get update
       sudo apt-get install build-essential cmake qt6-base-dev qt6-webengine-dev
       ```

   *   **Arch Linux:**
       ```bash
       sudo pacman -S base-devel cmake qt6-base qt6-webengine qt6-svg
       ```

2. **Build and Install:**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   sudo make install
   ```

## Running the Browser

Once installed, you can run the browser from your terminal or application menu:

```bash
udo
```

### Command Line Arguments
- `--configdir=<path>`: Specify an alternative persistent configuration directory.
- `<url>`: Specify an initial URL to open.

**Example:**
```bash
udo --configdir=~/.config/udo-testing/ github.com
```

## Configuration

On the first run, µDo! creates a configuration directory at `~/.config/udo/` (or your custom path) and copies the default `config.json` there. 

You can edit `config.json` to customize:
*   **Theme:** Colors, fonts, borders, and transparency for every UI component.
*   **Behavior:** Start page, search engines, HTTPS priority, and default download path.
*   **Commands:** Custom "bangs" (`!`, `!!`, `!!!`) and keyboard shortcuts.
*   **Chromium Flags:** Pass low-level flags (e.g., `--force-dark-mode`) to the engine.

## Key Features
*   **Do!-bar:** A centralized command input (`Ctrl+D`).
*   **Standalone Tab Bar:** Configurable positioning (top/bottom) and styling.
*   **Persistent Sessions:** Cookies and logins are saved automatically.
*   **HTTP Auth:** Robust dialog for authenticated sites.
*   **Local Content:** Support for loading local HTML, PDF, and image files.
