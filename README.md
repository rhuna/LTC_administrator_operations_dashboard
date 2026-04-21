# LTC Administrator Operations Dashboard v33 referral/document intake rebuild

Cross-platform desktop app starter built with **C++17**, **Qt 6 Widgets**, **Qt SQL**, **Qt PrintSupport**, **SQLite**, and **CMake**.

## v33 focus
- role-aware sign-in
- cleaner executive shell carried forward from v16+
- full admissions/discharges and staffing operations
- executive reports and export workspace
- all earlier modules preserved

## Demo users
- `admin` / `admin123` → Administrator
- `don` / `don123` → Director of Nursing
- `admissions` / `admit123` → Admissions Director
- `staffing` / `staff123` → Staffing Coordinator
- `viewer` / `view123` → Executive Viewer

## v33 highlights
- Added a login dialog before the main dashboard opens
- Added a seeded `users` table in SQLite
- Added role-aware navigation filtering by signed-in user
- Added signed-in user badge to the header
- Moved local SQLite file to `ltc_admin_dashboard_v33.db`

## Windows build (Qt 6.11 MinGW)
```bat
set QT_PREFIX=C:\Qt\6.11.0\mingw_64
set MINGW_BIN=C:\Qt\Tools\mingw1310_64\bin
build_release_and_run.bat clean
```

## Notes
- This is still a Qt desktop app.
- Earlier versions should remain intact as separate snapshots outside this repo folder.


## New in v33
- Workflow Center for editing, archiving, and deleting operational records
- Role-aware access now includes a cross-module cleanup workspace


## v33 additions
- Document Center module
- Add Document Item form
- Seeded document-tracking records for admissions, survey, and compliance


## v33 highlights
- Census Management / Stay Tracking module
- Bed hold, room move, payer change, and leave/readmit tracking
- Dashboard KPI for census events


## New in v33
- Survey Command Center module
- Survey command items table with seeded mock-survey and evidence tasks
- Dashboard and action-center visibility for survey follow-up


New in v33:
- Outbreak Command View module
- Seeded outbreak tracking records
- Dashboard visibility for outbreak workload
