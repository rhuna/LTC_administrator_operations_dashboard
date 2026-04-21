#include "DashboardCustomizePage.h"
#include "../../data/DatabaseManager.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>

DashboardCustomizePage::DashboardCustomizePage(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), m_db(db) {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(16);

    auto* hero = new QGroupBox(this);
    hero->setObjectName("heroPanel");
    auto* heroLayout = new QVBoxLayout(hero);
    heroLayout->setContentsMargins(20, 18, 20, 18);
    heroLayout->setSpacing(6);

    auto* title = new QLabel("Dashboard customization", hero);
    title->setObjectName("dashboardTitle");
    auto* subtitle = new QLabel(
        "Save a preferred landing page, adjust dashboard density, pin the modules you want leadership to see first, and keep a short executive focus note for the day.",
        hero);
    subtitle->setObjectName("dashboardSubtitle");
    subtitle->setWordWrap(true);
    heroLayout->addWidget(title);
    heroLayout->addWidget(subtitle);
    root->addWidget(hero);

    auto* settingsBox = new QGroupBox("Layout and launch preferences", this);
    auto* settingsLayout = new QFormLayout(settingsBox);

    m_defaultPageCombo = new QComboBox(settingsBox);
    m_defaultPageCombo->addItems({"Dashboard", "Search", "Alerts", "Residents", "Admissions", "Staffing", "Quality", "Survey Cmd", "Outbreak Command", "Reports"});

    m_densityCombo = new QComboBox(settingsBox);
    m_densityCombo->addItems({"Comfortable", "Compact", "Executive"});

    m_focusNoteEdit = new QLineEdit(settingsBox);
    m_focusNoteEdit->setPlaceholderText("Example: Watch staffing gaps, survey binder, and admit-ready referrals.");

    settingsLayout->addRow("Default page:", m_defaultPageCombo);
    settingsLayout->addRow("Dashboard density:", m_densityCombo);
    settingsLayout->addRow("Executive focus note:", m_focusNoteEdit);
    root->addWidget(settingsBox);

    auto* pinBox = new QGroupBox("Pin modules to the dashboard summary", this);
    auto* pinLayout = new QVBoxLayout(pinBox);
    auto* pinHint = new QLabel("Pinned modules appear in the dashboard customization callout so the leadership team can see today's preferred focus areas quickly.", pinBox);
    pinHint->setObjectName("panelHint");
    pinHint->setWordWrap(true);

    m_pinQuality = new QCheckBox("Quality", pinBox);
    m_pinAdmissions = new QCheckBox("Admissions", pinBox);
    m_pinStaffing = new QCheckBox("Staffing", pinBox);
    m_pinSurvey = new QCheckBox("Survey Cmd", pinBox);
    m_pinDocuments = new QCheckBox("Document Center", pinBox);
    m_pinOutbreak = new QCheckBox("Outbreak Command", pinBox);

    pinLayout->addWidget(pinHint);
    pinLayout->addWidget(m_pinQuality);
    pinLayout->addWidget(m_pinAdmissions);
    pinLayout->addWidget(m_pinStaffing);
    pinLayout->addWidget(m_pinSurvey);
    pinLayout->addWidget(m_pinDocuments);
    pinLayout->addWidget(m_pinOutbreak);
    root->addWidget(pinBox);

    auto* saveRow = new QHBoxLayout();
    auto* saveButton = new QPushButton("Save dashboard preferences", this);
    m_savedLabel = new QLabel("Not saved yet", this);
    m_savedLabel->setObjectName("panelHint");
    saveRow->addWidget(saveButton);
    saveRow->addWidget(m_savedLabel);
    saveRow->addStretch();
    root->addLayout(saveRow);

    auto* helpCard = new QFrame(this);
    helpCard->setObjectName("summaryStrip");
    auto* helpLayout = new QVBoxLayout(helpCard);
    helpLayout->setContentsMargins(16, 14, 16, 14);
    helpLayout->setSpacing(6);
    auto* helpTitle = new QLabel("How v39 uses these settings", helpCard);
    helpTitle->setStyleSheet("font-size:16px; font-weight:700; color:#102a43;");
    auto* helpBody = new QLabel(
        "The saved default page becomes your launch page, the density setting trims the executive copy on the dashboard, and pinned modules are surfaced as a quick reference strip.",
        helpCard);
    helpBody->setWordWrap(true);
    helpBody->setObjectName("panelHint");
    helpLayout->addWidget(helpTitle);
    helpLayout->addWidget(helpBody);
    root->addWidget(helpCard);
    root->addStretch();

    QObject::connect(saveButton, &QPushButton::clicked, this, [this]() { savePreferences(); });

    loadPreferences();
}

void DashboardCustomizePage::loadPreferences() {
    if (!m_db) return;
    const auto rows = m_db->fetchTable("dashboard_preferences", {"pref_key", "pref_value"});
    QMap<QString, QString> prefs;
    for (const auto& row : rows) prefs[row.value("pref_key")] = row.value("pref_value");

    const QString defaultPage = prefs.value("default_page", "Dashboard");
    const QString density = prefs.value("density_mode", "Comfortable");
    const QString pinned = prefs.value("pinned_modules");
    const QString note = prefs.value("focus_note");
    const QStringList pins = pinned.split(',', Qt::SkipEmptyParts);

    int pageIndex = m_defaultPageCombo->findText(defaultPage);
    if (pageIndex >= 0) m_defaultPageCombo->setCurrentIndex(pageIndex);
    int densityIndex = m_densityCombo->findText(density);
    if (densityIndex >= 0) m_densityCombo->setCurrentIndex(densityIndex);
    m_focusNoteEdit->setText(note);

    m_pinQuality->setChecked(pins.contains("Quality"));
    m_pinAdmissions->setChecked(pins.contains("Admissions"));
    m_pinStaffing->setChecked(pins.contains("Staffing"));
    m_pinSurvey->setChecked(pins.contains("Survey Cmd"));
    m_pinDocuments->setChecked(pins.contains("Document Center"));
    m_pinOutbreak->setChecked(pins.contains("Outbreak Command"));

    m_savedLabel->setText(rows.isEmpty() ? "Using default settings" : "Loaded saved dashboard preferences");
}

void DashboardCustomizePage::savePreferences() {
    if (!m_db) return;

    QStringList pins;
    if (m_pinQuality->isChecked()) pins << "Quality";
    if (m_pinAdmissions->isChecked()) pins << "Admissions";
    if (m_pinStaffing->isChecked()) pins << "Staffing";
    if (m_pinSurvey->isChecked()) pins << "Survey Cmd";
    if (m_pinDocuments->isChecked()) pins << "Document Center";
    if (m_pinOutbreak->isChecked()) pins << "Outbreak Command";

    const auto rows = m_db->fetchTable("dashboard_preferences", {"id", "pref_key", "pref_value"});
    QMap<QString, int> existingIds;
    for (const auto& row : rows) existingIds[row.value("pref_key")] = row.value("id").toInt();

    auto upsertPref = [&](const QString& key, const QString& value) {
        QMap<QString, QString> values{{"pref_key", key}, {"pref_value", value}};
        if (existingIds.contains(key)) {
            m_db->updateRecordById("dashboard_preferences", existingIds.value(key), values);
        } else {
            m_db->addRecord("dashboard_preferences", values);
        }
    };

    upsertPref("default_page", m_defaultPageCombo->currentText());
    upsertPref("density_mode", m_densityCombo->currentText());
    upsertPref("focus_note", m_focusNoteEdit->text().trimmed());
    upsertPref("pinned_modules", pins.join(','));

    m_savedLabel->setText("Saved. Reopen the app window to see launch-page changes.");
}
