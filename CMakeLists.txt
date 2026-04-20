cmake_minimum_required(VERSION 3.21)
project(LTCAdministratorOperationsDashboard VERSION 12.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

qt_standard_project_setup()

set(APP_SOURCES
    src/main.cpp
    src/core/AppWindow.h
    src/core/AppWindow.cpp
    src/data/DatabaseManager.h
    src/data/DatabaseManager.cpp
    src/ui/widgets/KpiCard.h
    src/ui/widgets/KpiCard.cpp
    src/ui/pages/DashboardPage.h
    src/ui/pages/DashboardPage.cpp
    src/ui/pages/BudgetPage.h
    src/ui/pages/BudgetPage.cpp
    src/ui/pages/CompliancePage.h
    src/ui/pages/CompliancePage.cpp
    src/ui/pages/HuddlePage.h
    src/ui/pages/HuddlePage.cpp
    src/ui/pages/QualityMeasuresPage.h
    src/ui/pages/QualityMeasuresPage.cpp
    src/ui/pages/TrainingPage.h
    src/ui/pages/TrainingPage.cpp
    src/ui/pages/EmergencyPreparednessPage.h
    src/ui/pages/EmergencyPreparednessPage.cpp
    src/ui/pages/InfectionControlPage.h
    src/ui/pages/InfectionControlPage.cpp
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
    src/ui/pages/IncidentsPage.h
    src/ui/pages/IncidentsPage.cpp
    src/ui/pages/SurveyReadinessPage.h
    src/ui/pages/SurveyReadinessPage.cpp
    resources/resources.qrc
)

qt_add_executable(${PROJECT_NAME}
    WIN32
    MACOSX_BUNDLE
    ${APP_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Sql)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
