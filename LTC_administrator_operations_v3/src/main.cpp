#include <QApplication>
#include <QMessageBox>

#include "core/AppWindow.h"
#include "data/DatabaseManager.h"
#include "ui/dialogs/LoginDialog.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("LTC Administrator Operations Dashboard");
    app.setApplicationVersion("95.0.0");
    app.setStyleSheet(R"(
        QWidget {
            background: #f4f7fb;
            color: #1f2937;
            font-family: "Segoe UI", "Inter", sans-serif;
            font-size: 12px;
        }
        QMainWindow, QScrollArea, QStackedWidget, QTabWidget::pane {
            background: #f4f7fb;
        }

        QListWidget#compactInsightList {
            background: #ffffff;
            border: 1px solid #d8e1eb;
            border-radius: 14px;
            padding: 6px;
        }
        QListWidget#compactInsightList::item {
            border-bottom: 1px solid #edf2f7;
            padding: 8px 10px;
        }
        QListWidget#compactInsightList::item:last {
            border-bottom: none;
        }
        QFrame#appHeader, QFrame#sidebarCard, QGroupBox, QFrame#kpiCard, QFrame#loginCard, QFrame#summaryStrip, QGroupBox#heroPanel {
            background: #ffffff;
            border: 1px solid #d8e1eb;
            border-radius: 20px;
        }
        QFrame#appHeader {
            border: 1px solid #d4dee9;
        }
        QLabel#appTitle {
            font-size: 28px;
            font-weight: 700;
            color: #102a43;
        }
        QLabel#appSubtitle, QLabel#dashboardSubtitle, QLabel#panelHint, QLabel#sidebarHint {
            color: #52606d;
            font-size: 13px;
            line-height: 1.35em;
        }
        QLabel#sidebarHeading {
            font-size: 11px;
            font-weight: 700;
            color: #486581;
            text-transform: uppercase;
            letter-spacing: 0.12em;
        }
        QListWidget#sideNav {
            background: transparent;
            border: none;
            outline: none;
            padding: 2px;
        }
        QListWidget#sideNav::item {
            background: #fbfcfe;
            border: 1px solid #e7eef5;
            border-radius: 12px;
            padding: 10px 12px;
            margin: 3px 0px;
            min-height: 22px;
            color: #243b53;
        }
        QListWidget#sideNav::item:hover {
            background: #f3f8fd;
            border: 1px solid #cfe0f2;
        }
        QListWidget#sideNav::item:selected {
            background: #dcecff;
            border: 1px solid #9fc5f8;
            color: #0b4f8a;
            font-weight: 700;
        }
        QLabel#dashboardTitle {
            font-size: 26px;
            font-weight: 700;
            color: #102a43;
        }
        QLabel#dashboardSnapshot {
            color: #0b4f8a;
            font-weight: 600;
            background: #f5f9ff;
            border: 1px solid #dce9f8;
            border-radius: 12px;
            padding: 10px 12px;
        }
        QLabel#summaryValue {
            font-size: 21px;
            font-weight: 700;
            color: #102a43;
        }
        QFrame#kpiCard {
            min-height: 112px;
        }
        QLabel#kpiTitle {
            font-size: 11px;
            font-weight: 700;
            color: #627d98;
            text-transform: uppercase;
            letter-spacing: 0.08em;
        }
        QLabel#kpiValue {
            font-size: 30px;
            font-weight: 700;
            color: #102a43;
        }
        QGroupBox {
            margin-top: 8px;
            padding-top: 16px;
            font-size: 15px;
            font-weight: 700;
            color: #102a43;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 6px;
        }
        QLineEdit, QTextEdit, QTableWidget, QListWidget#actionList, QComboBox, QDateEdit {
            background: #ffffff;
            border: 1px solid #d9e2ec;
            border-radius: 10px;
            padding: 7px 9px;
        }
        QLineEdit:focus, QTextEdit:focus, QComboBox:focus, QDateEdit:focus {
            border: 1px solid #7fb3e6;
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
        QPushButton:pressed { background: #095b8d; }
        QHeaderView::section {
            background: #eef4f8;
            color: #334e68;
            border: none;
            border-bottom: 1px solid #d9e2ec;
            padding: 8px;
            font-weight: 700;
        }
        QTableWidget {
            gridline-color: #e5edf5;
            selection-background-color: #d9eafc;
            alternate-background-color: #f8fbff;
            border-radius: 12px;
        }
        QTabWidget::pane {
            border: 1px solid #d8e1eb;
            border-radius: 18px;
            background: #ffffff;
            margin-top: 10px;
        }
        QTabBar::tab {
            background: #f6f9fc;
            border: 1px solid #d8e1eb;
            border-bottom: none;
            padding: 10px 14px;
            margin-right: 6px;
            border-top-left-radius: 12px;
            border-top-right-radius: 12px;
            color: #334e68;
            font-weight: 600;
        }
        QTabBar::tab:selected {
            background: #ffffff;
            color: #0b4f8a;
        }
        QTabBar::tab:hover {
            background: #edf5ff;
        }
        QScrollBar:vertical {
            background: transparent;
            width: 12px;
            margin: 4px;
        }
        QScrollBar::handle:vertical {
            background: #c9d8e8;
            border-radius: 6px;
            min-height: 32px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical,
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
            height: 0px;
        }
    )");

    DatabaseManager db;
    if (!db.initialize()) {
        QMessageBox::critical(
            nullptr,
            "Database Error",
            QString("Failed to initialize local SQLite database.\n\n%1").arg(db.lastErrorText()));
        return 1;
    }

    LoginDialog login(&db);
    if (login.exec() != QDialog::Accepted) {
        return 0;
    }

    AppWindow window(&db, login.fullName(), login.role());
    window.resize(1400, 900);
    window.show();
    return app.exec();
}
