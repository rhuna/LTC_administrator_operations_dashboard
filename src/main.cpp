#include <QApplication>
#include <QMessageBox>
#include "core/AppWindow.h"
#include "data/DatabaseManager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("LTC Administrator Operations Dashboard");
    app.setApplicationVersion("23.0.0");
    app.setStyleSheet(R"(
        QWidget {
            background: #f4f7fb;
            color: #1f2937;
            font-family: "Segoe UI", "Inter", sans-serif;
            font-size: 13px;
        }
        QMainWindow, QScrollArea, QStackedWidget {
            background: #f4f7fb;
        }
        QFrame#appHeader, QFrame#sidebarCard, QGroupBox, QFrame#kpiCard {
            background: #ffffff;
            border: 1px solid #d9e2ec;
            border-radius: 16px;
        }
        QLabel#appTitle {
            font-size: 26px;
            font-weight: 700;
            color: #102a43;
        }
        QLabel#appSubtitle, QLabel#dashboardSubtitle, QLabel#panelHint, QLabel#sidebarHint {
            color: #52606d;
            font-size: 13px;
        }
        QLabel#sidebarHeading {
            font-size: 12px;
            font-weight: 700;
            color: #486581;
            text-transform: uppercase;
            letter-spacing: 0.08em;
        }
        QListWidget#sideNav {
            background: transparent;
            border: none;
            outline: none;
            padding: 2px;
        }
        QListWidget#sideNav::item {
            background: #f8fafc;
            border: 1px solid #e5edf5;
            border-radius: 12px;
            padding: 10px 12px;
            margin: 2px 0px;
        }
        QListWidget#sideNav::item:selected {
            background: #d9eafc;
            border: 1px solid #9fc5f8;
            color: #0b4f8a;
            font-weight: 600;
        }
        QLabel#dashboardTitle {
            font-size: 24px;
            font-weight: 700;
            color: #102a43;
        }
        QLabel#dashboardSnapshot {
            color: #0b4f8a;
            font-weight: 600;
        }
        QFrame#kpiCard {
            min-height: 110px;
        }
        QLabel#kpiTitle {
            font-size: 12px;
            font-weight: 600;
            color: #627d98;
            text-transform: uppercase;
        }
        QLabel#kpiValue {
            font-size: 30px;
            font-weight: 700;
            color: #102a43;
        }
        QGroupBox {
            margin-top: 8px;
            padding-top: 14px;
            font-size: 15px;
            font-weight: 700;
            color: #102a43;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 6px;
        }
        QLineEdit, QTextEdit, QTableWidget, QListWidget#actionList {
            background: #ffffff;
            border: 1px solid #d9e2ec;
            border-radius: 10px;
            padding: 6px 8px;
        }
        QPushButton {
            background: #0b69a3;
            color: white;
            border: none;
            border-radius: 10px;
            padding: 9px 14px;
            font-weight: 600;
        }
        QPushButton:hover { background: #0f7cb8; }
        QHeaderView::section {
            background: #eef4f8;
            color: #334e68;
            border: none;
            border-bottom: 1px solid #d9e2ec;
            padding: 8px;
            font-weight: 600;
        }
        QTableWidget {
            gridline-color: #e5edf5;
            selection-background-color: #d9eafc;
            alternate-background-color: #f8fbff;
        }
    )");

    DatabaseManager db;
    if (!db.initialize()) {
        QMessageBox::critical(nullptr, "Database Error", "Failed to initialize local SQLite database.");
        return 1;
    }

    AppWindow window(&db);
    window.resize(1400, 900);
    window.show();
    return app.exec();
}
