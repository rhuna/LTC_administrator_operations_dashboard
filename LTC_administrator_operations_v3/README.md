# LTC Administrator Operations Dashboard v88 - Cross-Page Shared Records

## v88 Cross-Page Shared Records

This version builds on the connected workspace and live-refresh foundation by adding a shared-record bridge layer. Related items can now be represented as cross-page records so one operational issue is visible in multiple relevant tabs inside the same global workspace.

### What is new in v88
- Added a new `shared_record_links` SQLite table with seeded cross-page linkage examples.
- Added DatabaseManager helpers for shared-record retrieval and workspace-level shared-record highlights.
- Updated the global insight panels so each hub now shows both:
  - connected summary rollups
  - cross-page shared records that should stay visible in multiple tabs
- Updated app and database versioning to **88.0.0**.
- Updated the local SQLite filename to a **v88-specific** database path.

### Shared-record examples included
- Daily operations barrier visible in pulse, huddle, and alerts
- Executive follow-up visible from rounds into huddle
- Survey request linked across command center, documents, exports, and alerts
- Resident tracer linked into plan of correction and survey command
- Resident incident linked across care tabs
- Support task linked across tasks, calendar, and alerts
- Document packet linked across documents, reports, and setup
