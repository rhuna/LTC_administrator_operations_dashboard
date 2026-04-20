@echo off
setlocal EnableDelayedExpansion
cd /d "%~dp0"
set "APP_NAME=LTCAdministratorOperationsDashboard"
set "BUILD_DIR=build"
if /I "%~1"=="clean" (
    echo Cleaning old build folder...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)
if not defined QT_PREFIX set "QT_PREFIX=C:\Qt\6.11.0\mingw_64"
if not defined MINGW_BIN set "MINGW_BIN=C:\Qt\Tools\mingw1310_64\bin"
if not exist "%QT_PREFIX%\lib\cmake\Qt6\Qt6Config.cmake" (
    echo ERROR: Qt6Config.cmake not found at:
    echo   %QT_PREFIX%\lib\cmake\Qt6\Qt6Config.cmake
    exit /b 1
)
if not exist "%MINGW_BIN%\g++.exe" (
    echo ERROR: g++.exe not found at:
    echo   %MINGW_BIN%\g++.exe
    exit /b 1
)
echo Using QT_PREFIX=%QT_PREFIX%
echo Using MINGW_BIN=%MINGW_BIN%
set "PATH=%MINGW_BIN%;%QT_PREFIX%\bin;%PATH%"
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
echo.
echo ========================================
echo Configuring Release build...
echo ========================================
cmake -S . -B "%BUILD_DIR%" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QT_PREFIX%" -DCMAKE_CXX_COMPILER="%MINGW_BIN%\g++.exe"
if errorlevel 1 exit /b 1
echo.
echo ========================================
echo Building Release...
echo ========================================
cmake --build "%BUILD_DIR%" -j
if errorlevel 1 exit /b 1
set "EXE_PATH=%BUILD_DIR%\%APP_NAME%.exe"
if not exist "%EXE_PATH%" (
    echo ERROR: Built executable not found.
    exit /b 1
)
if exist "%QT_PREFIX%\bin\windeployqt.exe" "%QT_PREFIX%\bin\windeployqt.exe" "%EXE_PATH%"
start "" "%EXE_PATH%"
