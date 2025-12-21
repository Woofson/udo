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
