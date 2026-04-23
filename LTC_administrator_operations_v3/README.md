# LTC Administrator Operations Dashboard v95 Executive UI Final Polish

## v95 Executive UI Final Polish

### What is new in v95
- Polished the executive presentation across the app without removing functionality.
- Cleaned dashboard wording so the home screen reads more clearly.
- Tightened role-based view wording so each lens scans faster.
- Refined the stylesheet for calmer spacing, better contrast, and cleaner executive cards, tabs, and tables.
- Updated app and database versioning to **95.0.0**.
- Updated the local SQLite filename to a **v95-specific** database path.

### Main files updated
- `resources/app.qss`
- `src/ui/pages/DashboardPage.cpp`
- `src/ui/pages/RoleBasedExecutiveViewsPage.cpp`
- `src/ui/pages/ReportsPage.cpp`
- `src/core/AppWindow.cpp`
- `src/data/DatabaseManager.cpp`
- `src/main.cpp`
- `CMakeLists.txt`

### Notes
- This version is a final polish pass for the current build line.
- Existing workspaces, connected summaries, shared records, unified action center, shared notes, reporting, and role-based views remain intact.
