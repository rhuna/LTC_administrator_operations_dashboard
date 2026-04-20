# LTC Administrator Operations Dashboard v16

Cross-platform desktop app starter built with **C++17**, **Qt 6 Widgets**, **Qt SQL**, **SQLite**, and **CMake**.

## v16 focus
This snapshot keeps the full module set intact, but simplifies the experience with a cleaner shell:
- calmer styling and spacing
- lighter-weight sidebar navigation
- dashboard redesigned for easier scanning
- scroll-friendly layout preserved for smaller displays
- same functional modules as the prior snapshot

## Included modules
- Dashboard
- Residents
- Admissions
- Staffing + staffing changes
- Tasks
- QAPI / PIP
- Budget / labor
- Compliance calendar
- Daily ops huddle
- Incidents
- Survey readiness
- Quality measures
- Managed care / billing
- Workforce credentialing
- Emergency preparedness
- Infection control
- Risk management / grievances
- Environmental rounds / plant operations

## Windows build (Qt 6.11 MinGW)
```bat
set QT_PREFIX=C:\Qt\6.11.0\mingw_64
set MINGW_BIN=C:\Qt\Tools\mingw1310_64\bin
build_release_and_run.bat clean
```

## Notes
- Local SQLite database file is created next to the executable as `ltc_admin_dashboard_v16.db`.
- This is still a Qt desktop app.
- Earlier versions should remain intact as separate snapshots outside this repo folder.
