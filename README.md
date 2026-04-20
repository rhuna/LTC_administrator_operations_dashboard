# LTC Administrator Operations Dashboard v12

A cross-platform **C++ desktop application** for long-term care administrators. This repo uses **Qt Widgets**, **SQLite**, and **CMake** so it can build on **Windows, Linux, and macOS**.

## What's new in v12

- Added a dedicated **Emergency Preparedness and Drills** module
- Added a **preparedness follow-up add form**
- Added seeded preparedness records for:
  - severe-weather shelter-in-place drill follow-up
  - generator load and fuel continuity check
  - missing-resident elopement tabletop review
  - evacuation transportation roster audit
- Added a new dashboard KPI: **Preparedness Items Due**
- Expanded the **Administrator Action Center** so overdue and due-soon preparedness items appear with the other executive follow-up risks
- Corrected the database table-creation chain so the new snapshot carries forward a cleaner initialization path
- Kept prior modules intact, including the **Qt 6.11 MinGW** Windows build flow

## Included modules

- Executive dashboard
- Residents / census view
- Admissions and transition pipeline
- Staffing and labor snapshot
- Staffing change tracking with add form
- Task tracker with add form
- QAPI / PIP tracker with add form
- Budget and labor variance tracker with add form
- Compliance calendar and license tracker with add form
- Daily operations huddle tracker with add form
- Quality measures and clinical outcomes tracker with add form
- Workforce development and credentialing tracker with add form
- Emergency preparedness and drills tracker with add form
- Incident log with add form
- Survey readiness tracker

## Tech stack

- C++17
- Qt 6 Widgets
- Qt 6 SQL
- SQLite
- CMake 3.21+

## Project structure

```text
LTCAdministratorOperationsDashboard/
├─ build_release_and_run.bat
├─ CMakeLists.txt
├─ README.md
├─ resources/
│  ├─ app.qss
│  └─ resources.qrc
├─ scripts/
│  ├─ build_windows_msvc.bat
│  ├─ build_linux.sh
│  └─ build_macos.sh
└─ src/
   ├─ core/
   ├─ data/
   └─ ui/
```

## Windows build for your setup (Qt 6.11 MinGW)

```bat
set QT_PREFIX=C:\Qt\6.11.0\mingw_64
set MINGW_BIN=C:\Qt\Tools\mingw1310_64\bin
build_release_and_run.bat clean
```

## If you need to confirm your Qt path

```bat
dir C:\Qt /s /b Qt6Config.cmake
```

## If you need to confirm your compiler path

```bat
dir C:\Qt\Tools\mingw1310_64\bin\g++.exe
```

## Linux build

```bash
chmod +x scripts/build_linux.sh
./scripts/build_linux.sh
```

## macOS build

```bash
chmod +x scripts/build_macos.sh
./scripts/build_macos.sh
```

## Data storage

The application stores its local SQLite database in the writable app-data location returned by Qt. On first launch it creates the database and seeds sample LTC operations data.

## Suggested v12 direction

- Editable resident and staffing records
- Admissions conversion funnel metrics
- User roles and authentication
- Export to CSV / PDF
- Survey binder document center
- Executive report printing
