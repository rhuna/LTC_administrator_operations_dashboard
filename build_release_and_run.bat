@echo off
setlocal EnableDelayedExpansion

REM ============================================================
REM LTC Administrator Operations Dashboard
REM MinGW Release Build + Run Script
REM Cross-platform Qt6/CMake project on Windows using MSYS2/Qt MinGW
REM ============================================================

cd /d "%~dp0"

set "APP_NAME=LTCAdministratorOperationsDashboard"
set "BUILD_DIR=build_mingw"

REM ------------------------------------------------------------
REM Optional clean rebuild
REM ------------------------------------------------------------
if /I "%~1"=="clean" (
    echo Cleaning old build folder...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

REM ------------------------------------------------------------
REM Try to locate Qt MinGW kit
REM User can override by setting QT_PREFIX before running:
REM   set QT_PREFIX=C:\Qt\6.9.0\mingw_64
REM ------------------------------------------------------------
if defined QT_PREFIX goto qt_check

for %%D in (
    C:\Qt\6.9.0\mingw_64
    C:\Qt\6.8.0\mingw_64
    C:\Qt\6.7.3\mingw_64
    C:\Qt\6.7.2\mingw_64
    C:\Qt\6.6.3\mingw_64
    C:\Qt\6.6.2\mingw_64
) do (
    if exist "%%~D\lib\cmake\Qt6\Qt6Config.cmake" (
        set "QT_PREFIX=%%~D"
        goto qt_check
    )
)

echo ERROR: Could not find a Qt MinGW installation.
echo.
echo Set QT_PREFIX manually, for example:
echo   set QT_PREFIX=C:\Qt\6.8.0\mingw_64
echo   build_release_and_run_mingw.bat
echo.
echo To find your Qt path, run:
echo   dir C:\Qt /s /b Qt6Config.cmake
exit /b 1

:qt_check
if not exist "%QT_PREFIX%\lib\cmake\Qt6\Qt6Config.cmake" (
    echo ERROR: Qt6Config.cmake not found at:
    echo   %QT_PREFIX%\lib\cmake\Qt6\Qt6Config.cmake
    exit /b 1
)

echo Using QT_PREFIX=%QT_PREFIX%

REM ------------------------------------------------------------
REM Try to locate MinGW compiler
REM User can override by setting MINGW_BIN before running:
REM   set MINGW_BIN=C:\Qt\Tools\mingw1310_64\bin
REM or:
REM   set MINGW_BIN=C:\msys64\ucrt64\bin
REM ------------------------------------------------------------
if defined MINGW_BIN goto mingw_check

for %%D in (
    C:\Qt\Tools\mingw1310_64\bin
    C:\Qt\Tools\mingw1120_64\bin
    C:\Qt\Tools\mingw1100_64\bin
    C:\Qt\Tools\mingw900_64\bin
    C:\msys64\ucrt64\bin
    C:\msys64\mingw64\bin
) do (
    if exist "%%~D\g++.exe" (
        set "MINGW_BIN=%%~D"
        goto mingw_check
    )
)

echo ERROR: Could not find MinGW g++.
echo.
echo Set MINGW_BIN manually, for example:
echo   set MINGW_BIN=C:\Qt\Tools\mingw1310_64\bin
echo or
echo   set MINGW_BIN=C:\msys64\ucrt64\bin
exit /b 1

:mingw_check
if not exist "%MINGW_BIN%\g++.exe" (
    echo ERROR: g++.exe not found in:
    echo   %MINGW_BIN%
    exit /b 1
)

echo Using MINGW_BIN=%MINGW_BIN%

set "PATH=%MINGW_BIN%;%QT_PREFIX%\bin;%PATH%"

REM ------------------------------------------------------------
REM Ensure CMake exists
REM ------------------------------------------------------------
where cmake >nul 2>nul
if errorlevel 1 (
    echo ERROR: cmake not found in PATH.
    exit /b 1
)

REM ------------------------------------------------------------
REM Configure
REM ------------------------------------------------------------
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo.
echo ========================================
echo Configuring MinGW Release build...
echo ========================================
cmake -S . -B "%BUILD_DIR%" ^
    -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="%QT_PREFIX%" ^
    -DCMAKE_C_COMPILER="%MINGW_BIN%\gcc.exe" ^
    -DCMAKE_CXX_COMPILER="%MINGW_BIN%\g++.exe"

if errorlevel 1 (
    echo.
    echo ERROR: CMake configure failed.
    exit /b 1
)

REM ------------------------------------------------------------
REM Build
REM ------------------------------------------------------------
echo.
echo ========================================
echo Building Release...
echo ========================================
cmake --build "%BUILD_DIR%" --config Release -j

if errorlevel 1 (
    echo.
    echo ERROR: Build failed.
    exit /b 1
)

REM ------------------------------------------------------------
REM Find EXE
REM ------------------------------------------------------------
set "EXE_PATH=%BUILD_DIR%\%APP_NAME%.exe"
if not exist "%EXE_PATH%" (
    for /r "%BUILD_DIR%" %%F in (*.exe) do (
        if /I "%%~nxF"=="%APP_NAME%.exe" set "EXE_PATH=%%~fF"
    )
)

if not exist "%EXE_PATH%" (
    echo ERROR: Could not find built executable.
    exit /b 1
)

echo.
echo Built executable:
echo   %EXE_PATH%

REM ------------------------------------------------------------
REM Deploy Qt runtime if windeployqt exists
REM ------------------------------------------------------------
set "WINDEPLOYQT=%QT_PREFIX%\bin\windeployqt.exe"
if exist "%WINDEPLOYQT%" (
    echo.
    echo ========================================
    echo Deploying Qt runtime...
    echo ========================================
    "%WINDEPLOYQT%" "%EXE_PATH%"
) else (
    echo WARNING: windeployqt not found, skipping deployment.
)

REM ------------------------------------------------------------
REM Run
REM ------------------------------------------------------------
echo.
echo ========================================
echo Launching application...
echo ========================================
start "" "%EXE_PATH%"

exit /b 0