# LTC Administrator Operations Dashboard

A cross-platform **C++ desktop application** for long-term care administrators. This starter repo uses **Qt Widgets**, **SQLite**, and **CMake** so it can be built on **Windows, Linux, and macOS**.

## What it includes

- Executive dashboard with KPI cards
- Residents / census table
- Staffing and labor table
- Task tracker with add form
- Incident log with add form
- Survey readiness tracker
- Local SQLite database with sample seed data
- Cross-platform CMake project structure

## Tech stack

- C++17
- Qt 6 Widgets
- Qt 6 SQL
- SQLite
- CMake 3.21+

## Project structure

```text
LTCAdministratorOperationsDashboard/
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

## Build requirements

### Windows
- Visual Studio 2022 or newer
- CMake 3.21+
- Qt 6 with Widgets and Sql components installed

Example Qt install path:
- `C:/Qt/6.8.0/msvc2022_64`

### Linux
- GCC or Clang
- CMake 3.21+
- Qt 6 development packages

Ubuntu example:
```bash
sudo apt update
sudo apt install -y build-essential cmake qt6-base-dev
```

### macOS
- Xcode command line tools
- CMake 3.21+
- Qt 6 installed via Qt online installer or Homebrew

Homebrew example:
```bash
brew install cmake qt
```

## Build instructions

### Windows (MSVC)
```bat
scripts\build_windows_msvc.bat
```

Or manually:
```bat
cmake -S . -B build -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/msvc2022_64"
cmake --build build --config Release
```

### Linux
```bash
chmod +x scripts/build_linux.sh
./scripts/build_linux.sh
```

### macOS
```bash
chmod +x scripts/build_macos.sh
./scripts/build_macos.sh
```

## Run

### Windows
```bat
build\Release\LTCAdministratorOperationsDashboard.exe
```

### Linux / macOS
```bash
./build/LTCAdministratorOperationsDashboard
```

## Data storage

The application stores its local SQLite database in the app data location returned by Qt. On first launch it creates the database and seeds sample LTC operations data.

## Next version ideas

- Login and role-based permissions
- QAPI / PIP module
- Admissions and discharge workflow
- Budget and labor variance analytics
- Pharmacy / wound / infection control widgets
- Report export to PDF / CSV
- Editable resident and staffing records
- Survey binder document center



## Windows one-step build and run

From the repo root:

```bat
build_release_and_run.bat
```

To force a clean rebuild:

```bat
build_release_and_run.bat clean
```

If Qt is installed in a non-default location, set `QT_PREFIX` first:

```bat
set QT_PREFIX=C:\Qt\6.8.0\msvc2022_64
build_release_and_run.bat
```
