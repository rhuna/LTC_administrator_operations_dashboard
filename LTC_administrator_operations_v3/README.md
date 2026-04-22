# LTC Administrator Operations Dashboard

## v81.0.0 — Alerts & Escalation Center

### What is new in v81
- Added a new **Alerts & Escalation** page for logging, triaging, and resolving urgent cross-board issues.
- Added a new **alerts_escalation_items** SQLite table with seeded sample rows.
- Wired the new page into the sidebar navigation.
- Updated the **Survey Command Center** so it includes alert-center counts, critical-alert counts, blocked-alert counts, and due-now urgency rollups.
- Updated the main dashboard snapshot, summary strip, KPI cards, and quick-glance list to include alert-center workload.
- Updated app/window/version labeling to **81.0.0**.
- Updated the local SQLite database filename to a **v81-specific** path.

### v81 focus
This version adds an urgency layer across the survey workflow so leadership can see what is overdue, blocked, critical, or due right now before the next huddle, packet pull, tracer follow-up, or surveyor request handoff.
