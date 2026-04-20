# LTC Administrator Operations Dashboard v23

Cross-platform desktop app starter built with **C++17**, **Qt 6 Widgets**, **Qt SQL**, **SQLite**, and **CMake**.

## v23 focus
- cleaner executive UI carried forward from v16+
- real admissions/discharges and staffing operations
- minimum staffing visibility with gap highlighting
- bed-board, transport, and pharmacy follow-up in one command center
- estimated staffing hours and uncovered minimum hours
- nursing HPRD-style staffing summary for faster admin review

This snapshot keeps the full module set intact, but simplifies the experience with a cleaner shell:
- calmer styling and spacing
- lighter-weight sidebar navigation
- dashboard designed for easier scanning
- scroll-friendly layout preserved for smaller displays
- same functional modules as the prior snapshot, plus pharmacy / medication systems

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
- Bed board / room turnover
- Transportation / appointments
- Pharmacy / medication systems

## Windows build (Qt 6.11 MinGW)
```bat
set QT_PREFIX=C:\Qt.11.0\mingw_64
set MINGW_BIN=C:\Qt\Tools\mingw1310_64in
build_release_and_run.bat clean
```

## Notes
- Local SQLite database file is created next to the executable as `ltc_admin_dashboard_v23.db`.
- This is still a Qt desktop app.
- Earlier versions should remain intact as separate snapshots outside this repo folder.

## v23 highlights
- Added a Pharmacy / Medication Systems module
- Added seeded medication-delivery, authorization, and controlled-drug review items
- Surfaced pharmacy follow-up on the executive dashboard and action center
