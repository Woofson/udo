# µDo! Release Log

## v2.0.2
**Release Date: 2025-12-17**

### Rebranding & Portability
- **New Identity:** Project officially renamed to **µDo!**.
- **Standalone Binary:** Portability improved by embedding assets and default configuration as Qt resources.
- **Improved Paths:** Cleaned up internal naming and simplified standard configuration paths to `~/.config/udo/`.

### Theming & UI
- **Modular Theme System:** Introduced a fully customizable theme engine in `config.json`.
- **Transparency Support:** Added support for RGBA background colors across all UI components.
- **Standalone Tab Bar:** Replaced default tab widget appearance with a highly configurable standalone bar that can be positioned at the top or bottom.
- **Styled Context Menu:** The right-click menu now follows the application's global theme.
- **Granular Border Control:** Every component now has configurable border sizes and colors.

### Core Browser Features
- **Persistent Storage:** Fixed critical issues with cookie and session handling using explicit profile management.
- **Configurable Downloads:** Added a "Save As..." dialog and user-definable default download paths.
- **Prioritize HTTPS:** New configurable option to automatically upgrade insecure connections.
- **HTTP Authentication:** Overhauled auth dialogs with error feedback and full support for international/special characters.
- **Local Content:** Enhanced support for loading local machine files, including PDFs, images, and HTML.
- **Command Line Navigation:** Added support for launching the browser directly to a specific URL or using alternative configuration profiles.

### Technical Improvements
- **Chromium Flags:** Added the ability to pass engine-level flags (like `--force-dark-mode`) directly from the configuration file.
- **Shortcut Enhancements:** New shortcuts for page refresh (F5, Ctrl+R) and hiding bars (Escape).
- **Stability Fixes:** Resolved initialization order conflicts that caused startup crashes.
- **Build System:** Updated `CMakeLists.txt` with better asset handling and support for Arch-based distributions.

---

## v2.0.1-rc1
- Initial rename to µDo!
- Implemented basic persistent storage logic.
- Added preliminary dark mode CSS injection.

## v2.0.0
- Initial release of the redesigned, command-driven browser concept.
- Introduced the "Do!-bar" and "Do!-menu".
- Basic support for bang (!) commands.
