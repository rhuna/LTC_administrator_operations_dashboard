cmake_minimum_required(VERSION 3.21)
project(LTCAdministratorOperationsDashboard VERSION 33.0.0 LANGUAGES CXX)

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
