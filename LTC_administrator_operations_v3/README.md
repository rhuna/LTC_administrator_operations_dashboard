# LTC Administrator Operations Dashboard v94 Role-Based Executive Views

## v94 Role-Based Executive Views

### What is new in v94
- Added a dedicated **Role-Based Executive Views** page.
- Added focused executive lenses for:
  - **Administrator**
  - **DON / Clinical**
  - **Department Leadership**
  - **Survey Response Lead**
- Kept the connected workspace, action-center, shared notes, and reporting model intact.
- Added role-specific priority summaries, follow-up guidance, and board-level pressure rollups without removing any existing functionality.
- Updated app and database versioning to **94.0.0**.
- Updated the local SQLite filename to a **v94-specific** database path.

### Main files added
- `src/ui/pages/RoleBasedExecutiveViewsPage.h`
- `src/ui/pages/RoleBasedExecutiveViewsPage.cpp`

### Main files updated
- `src/core/AppWindow.cpp`
- `src/data/DatabaseManager.cpp`
- `src/main.cpp`
- `CMakeLists.txt`

### Notes
- This snapshot keeps the simplified executive home screen, unified action center, shared notes, connected workspaces, and reporting/export tools intact.
- The new role-based page does not remove permissions or hide data. It gives leadership different operational lenses over the same connected system.
