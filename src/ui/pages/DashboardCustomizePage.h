#pragma once
#include <QWidget>

class DatabaseManager;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QLabel;

class DashboardCustomizePage : public QWidget {
public:
    explicit DashboardCustomizePage(DatabaseManager* db, QWidget* parent = nullptr);

private:
    void loadPreferences();
    void savePreferences();

    DatabaseManager* m_db{nullptr};
    QLabel* m_savedLabel{nullptr};
    QComboBox* m_defaultPageCombo{nullptr};
    QComboBox* m_densityCombo{nullptr};
    QLineEdit* m_focusNoteEdit{nullptr};
    QCheckBox* m_pinQuality{nullptr};
    QCheckBox* m_pinAdmissions{nullptr};
    QCheckBox* m_pinStaffing{nullptr};
    QCheckBox* m_pinSurvey{nullptr};
    QCheckBox* m_pinDocuments{nullptr};
    QCheckBox* m_pinOutbreak{nullptr};
};
