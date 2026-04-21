cmake_minimum_required(VERSION 3.21)
project(LTCAdministratorOperationsDashboard VERSION 55.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql PrintSupport)

qt_add_executable(LTCAdministratorOperationsDashboard
    src/main.cpp
    src/core/AppWindow.h
    src/core/AppWindow.cpp
    src/data/DatabaseManager.h
    src/data/DatabaseManager.cpp
    src/ui/dialogs/LoginDialog.h
    src/ui/dialogs/LoginDialog.cpp
    src/ui/widgets/KpiCard.h
    src/ui/widgets/KpiCard.cpp
    src/ui/pages/DashboardPage.h
    src/ui/pages/DashboardPage.cpp
    src/ui/pages/AlertsPage.h
    src/ui/pages/AlertsPage.cpp
    src/ui/pages/SearchFiltersPage.h
    src/ui/pages/SearchFiltersPage.cpp
    src/ui/pages/DashboardCustomizePage.h
    src/ui/pages/DashboardCustomizePage.cpp
    src/ui/pages/CalendarPage.h
    src/ui/pages/CalendarPage.cpp
    src/ui/pages/MetricsChartsPage.h
    src/ui/pages/MetricsChartsPage.cpp
    src/ui/pages/KpiTrendEnginePage.h
    src/ui/pages/KpiTrendEnginePage.cpp
    src/ui/pages/ServiceLayerPage.h
    src/ui/pages/ServiceLayerPage.cpp
    src/ui/pages/ExternalSyncReadinessPage.h
    src/ui/pages/ExternalSyncReadinessPage.cpp
    src/ui/pages/ReleaseCandidatePage.h
    src/ui/pages/ReleaseCandidatePage.cpp
    src/ui/pages/SopCenterPage.h
    src/ui/pages/SopCenterPage.cpp
    src/ui/pages/ShiftHandoffPage.h
    src/ui/pages/ShiftHandoffPage.cpp
    src/ui/pages/CareConferencePage.h
    src/ui/pages/CareConferencePage.cpp
    src/ui/pages/TherapyRehabPage.h
    src/ui/pages/TherapyRehabPage.cpp
    src/ui/pages/SocialServicesPage.h
    src/ui/pages/SocialServicesPage.cpp
    src/ui/pages/DepartmentDashboardsPage.h
    src/ui/pages/DepartmentDashboardsPage.cpp
    src/ui/pages/BackupRestorePage.h
    src/ui/pages/BackupRestorePage.cpp
    src/ui/pages/AuditLogPage.h
    src/ui/pages/AuditLogPage.cpp
    src/ui/pages/FormsValidationPage.h
    src/ui/pages/FormsValidationPage.cpp
    src/ui/pages/ResidentsPage.h
    src/ui/pages/ResidentsPage.cpp
    src/ui/pages/AdmissionsPage.h
    src/ui/pages/AdmissionsPage.cpp
    src/ui/pages/StaffingPage.h
    src/ui/pages/StaffingPage.cpp
    src/ui/pages/TasksPage.h
    src/ui/pages/TasksPage.cpp
    src/ui/pages/QapiPage.h
    src/ui/pages/QapiPage.cpp
    src/ui/pages/BudgetPage.h
    src/ui/pages/BudgetPage.cpp
    src/ui/pages/CompliancePage.h
    src/ui/pages/CompliancePage.cpp
    src/ui/pages/HuddlePage.h
    src/ui/pages/HuddlePage.cpp
    src/ui/pages/IncidentsPage.h
    src/ui/pages/IncidentsPage.cpp
    src/ui/pages/SurveyReadinessPage.h
    src/ui/pages/SurveyReadinessPage.cpp
    src/ui/pages/QualityMeasuresPage.h
    src/ui/pages/QualityMeasuresPage.cpp
    src/ui/pages/ManagedCarePage.h
    src/ui/pages/ManagedCarePage.cpp
    src/ui/pages/CredentialingPage.h
    src/ui/pages/CredentialingPage.cpp
    src/ui/pages/PreparednessPage.h
    src/ui/pages/PreparednessPage.cpp
    src/ui/pages/InfectionControlPage.h
    src/ui/pages/InfectionControlPage.cpp
    src/ui/pages/RiskManagementPage.h
    src/ui/pages/RiskManagementPage.cpp
    src/ui/pages/EnvironmentalRoundsPage.h
    src/ui/pages/EnvironmentalRoundsPage.cpp
    src/ui/pages/BedBoardPage.h
    src/ui/pages/BedBoardPage.cpp
    src/ui/pages/TransportationPage.h
    src/ui/pages/TransportationPage.cpp
    src/ui/pages/PharmacyPage.h
    src/ui/pages/PharmacyPage.cpp
    src/ui/pages/DietaryPage.h
    src/ui/pages/DietaryPage.cpp
    src/ui/pages/ReportsPage.h
    src/ui/pages/ReportsPage.cpp
    src/ui/pages/WorkflowCenterPage.h
    src/ui/pages/WorkflowCenterPage.cpp
    src/ui/pages/DocumentCenterPage.h
    src/ui/pages/DocumentCenterPage.cpp
    src/ui/pages/CensusManagementPage.h
    src/ui/pages/CensusManagementPage.cpp
    src/ui/pages/MdsTripleCheckPage.h
    src/ui/pages/MdsTripleCheckPage.cpp
    src/ui/pages/SurveyCommandCenterPage.h
    src/ui/pages/SurveyCommandCenterPage.cpp
    src/ui/pages/OutbreakCommandPage.h
    src/ui/pages/OutbreakCommandPage.cpp
    resources/resources.qrc
)

target_link_libraries(LTCAdministratorOperationsDashboard PRIVATE Qt6::Widgets Qt6::Sql Qt6::PrintSupport)

install(TARGETS LTCAdministratorOperationsDashboard RUNTIME DESTINATION .)
