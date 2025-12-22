# µDo! Release Log

## v2.0.3
**Release Date: 2025-12-17**

### Theme & UI Finalization
- **Fixed Theming Inconsistencies:** Resolved issues with Do!-bar borders, button text, and menu highlighting by using unique QSS placeholders.
- **Tab Bar Refinement:** Eliminated the white "base" line at the bottom of the tab bar for a cleaner look.
- **Configurable Borders:** Re-introduced granular control over all component borders.
- **Themed About Page:** Updated the About dialog to correctly apply themed link colors.

### Portability & Standalone Features
- **Embedded Assets:** Application icons and the default configuration are now fully embedded as Qt resources, making the binary truly standalone.
- **Robust Icon Loading:** Improved the About dialog's icon detection logic to support multiple environments.

### Stability & Versioning
- **Version Bump:** Officially released as v2.0.3.
- **Clean Console Output:** Removed invalid QSS properties to eliminate startup warnings.

---

## v2.0.2
- Initial rebranding to µDo!.
- Added standalone tab bar and modular theme engine.
- Implemented configurable downloads and save dialog.
- Improved HTTP authentication and HTTPS prioritization.
- Added command-line flag for alternative config directories.

## v2.0.0
- Initial release of the redesigned, command-driven browser concept.
- Introduced the "Do!-bar" and "Do!-menu".