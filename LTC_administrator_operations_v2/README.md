
## v55 Shift Handoff Center

This snapshot intentionally skips the multi-facility direction for now and continues from the fixed v46 line.

### What is new
- Service Layer / Integration Readiness module
- `service_registry` SQLite table
- seeded service rows for admissions, staffing, documents, and reporting
- app version text updated to v55
- local database file updated to `ltc_admin_dashboard_v55_service_layer.db`

### Build
```bat
set QT_PREFIX=C:\Qt.11.0\mingw_64
set MINGW_BIN=C:\Qt\Tools\mingw1310_64in
build_release_and_run.bat clean
```


## v55 highlight
This snapshot adds an External Sync / EMR Readiness workspace so future PCC/EMR integration planning stays visible without changing the current single-facility workflow.


## New in v55
- SOP / Quick Start Center module
- seeded operating guides for admissions, staffing, reports, and backup workflows
- dashboard visibility for SOP items


## v55 additions
- Care Conferences / Family Communication module
- New local database file: `ltc_admin_dashboard_v55_care_conference.db`


## v55
- Added Therapy / Rehab Operations module
