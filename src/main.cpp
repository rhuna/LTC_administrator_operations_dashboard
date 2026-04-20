#include <QApplication>
#include <QFile>
#include <QMessageBox>

#include "core/AppWindow.h"
#include "data/DatabaseManager.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("LTC Administrator Operations Dashboard");
    app.setOrganizationName("OpenAI Sample Projects");

    QFile styleFile(":/styles/app.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    DatabaseManager databaseManager;
    if (!databaseManager.initialize()) {
        QMessageBox::critical(nullptr,
                              "Database Error",
                              "The application could not initialize its local SQLite database.");
        return 1;
    }

    AppWindow window(&databaseManager);
    window.resize(1480, 900);
    window.show();

    return app.exec();
}
