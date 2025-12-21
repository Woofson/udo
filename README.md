# µDo!

Superminimalistic keyboard-driven web browser built on Qt6 WekKit and C++.  
No UI-clutter, no accounts, no sync, just web in focus.  

/T

µDo! Help
Configuration
Your configuration file is located at:
~/.config/udo/config.json  

Your browser data (cache, cookies, history) is also stored in this directory.  

## Bangs, twobangs and threebangs.

All of these are configurable in the config.

### !!! Commands
- !!!about: about
- !!!clc: clear_cookies
- !!!clh: clear_history
- !!!clx: clear_cache
- !!!help: help
- !!!max: maximize
- !!!min: minimize
- !!!norm: normalize
- !!!q: quit
- !!!tc: tab_close
- !!!tn: tab_new
- !!!tp: tab_prev
- !!!tx: tab_next

### !! Bookmarks
- !!gh: https://github.com
- !!home: https://dash.boop.no/
- !!qt: https://doc.qt.io/

### ! Searches
- !ddg: https://duckduckgo.com/?q=[query]
- !g: https://www.google.com/search?q=[query]
- !yt: https://www.youtube.com/results?search_query=[query]


### Shortcuts
- Ctrl+Shift+A: about
- Ctrl+T: new_tab
- Ctrl+Q: quit
- Ctrl+D: toggle_do_bar
- Ctrl+S: toggle_status_bar

See the example config.json for more details.  
µDo! can also be started with --configdir= for different config profiles.

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
