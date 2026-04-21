# LTC Administrator Operations Dashboard

## v59.0.0 — Financial & Billing Operations

**New pages:**
- **Revenue Cycle** — Track payer mix, billing queue items, denial follow-up, and A/R aging with add/resolve/delete capability and a live summary strip.
- **Contract Management** — Track vendor contracts (agency staffing, pharmacy, therapy, dietary) with renewal dates, rate schedules, escalation clause notes, and status workflow.

**Upgraded pages:**
- **Budget / Labor** — Rebuilt from a read-only stub into a full add/edit/delete workspace with department grouping, variance tracking, color-coded status, and a summary strip.
- **Managed Care** — Rebuilt with an authorization queue, auth expiry date column, color-coded status, and full add/approve/delete capability.

**Quality fixes:**
- Database file renamed from `v55_social_services` to `v59_financial_billing` to match the release.
- `ReportsPage` version strings corrected from hardcoded `v25` to `54.0.0`.
- `AppWindow` window title and subtitle updated to reflect v59 scope.
- CMakeLists bumped to `54.0.0`; missing `TrainingPage` source entries added.
- Duplicate `survey_command_items` re-seeding removed from `authenticateUser()` and `dischargeResident()` — seeding now happens only once in `seedData()`.
- `managed_care_items` schema migration added for new `auth_expiry` and `owner` columns on existing databases.
- Dashboard snapshot and action center updated to include Revenue Cycle and Contracts counts.

**Role access:**
- Administrator: full access to all pages including Revenue Cycle and Contracts.
- Director of Nursing: Revenue Cycle and Contracts added for financial visibility.
- Admissions Director: Revenue Cycle added.

---


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
