@echo off
setlocal EnableDelayedExpansion

cd /d "%~dp0"

set "APP_NAME=LTCAdministratorOperationsDashboard"
set "BUILD_DIR=build"

if /I "%~1"=="clean" (
    echo Cleaning old build folder...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

if not defined QT_PREFIX (
    for %%D in (
        C:\Qt\6.11.0\mingw_64
        C:\Qt\6.10.0\mingw_64
        C:\Qt\6.9.1\mingw_64
        C:\Qt\6.9.0\mingw_64
        C:\Qt\6.8.3\mingw_64
        C:\Qt\6.8.2\mingw_64
        C:\Qt\6.8.1\mingw_64
        C:\Qt\6.8.0\mingw_64
    ) do (
        if exist "%%~D\lib\cmake\Qt6\Qt6Config.cmake" (
            set "QT_PREFIX=%%~D"
            goto qt_found
        )
    )
) else (
    if exist "%QT_PREFIX%\lib\cmake\Qt6\Qt6Config.cmake" goto qt_found
)

echo ERROR: Qt6Config.cmake not found.
echo Find it with:
echo   dir C:\Qt /s /b Qt6Config.cmake
exit /b 1

:qt_found
echo Using QT_PREFIX=%QT_PREFIX%

if not defined MINGW_BIN (
    for %%D in (
        C:\Qt\Tools\mingw1310_64\bin
        C:\Qt\Tools\mingw1120_64\bin
        C:\Qt\Tools\mingw1100_64\bin
        C:\msys64\ucrt64\bin
        C:\msys64\mingw64\bin
    ) do (
        if exist "%%~D\g++.exe" (
            set "MINGW_BIN=%%~D"
            goto mingw_found
        )
    )
) else (
    if exist "%MINGW_BIN%\g++.exe" goto mingw_found
)

echo ERROR: MinGW g++ not found.
echo Set MINGW_BIN manually, for example:
echo   set MINGW_BIN=C:\Qt\Tools\mingw1310_64\bin
exit /b 1

:mingw_found
echo Using MINGW_BIN=%MINGW_BIN%

set "PATH=%MINGW_BIN%;%QT_PREFIX%\bin;%PATH%"

where cmake >nul 2>nul
if errorlevel 1 (
    echo ERROR: cmake not found in PATH.
    exit /b 1
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo.
echo ========================================
echo Configuring Release build...
echo ========================================
cmake -S . -B "%BUILD_DIR%" ^
  -G "MinGW Makefiles" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_PREFIX_PATH="%QT_PREFIX%" ^
  -DCMAKE_C_COMPILER="%MINGW_BIN%\gcc.exe" ^
  -DCMAKE_CXX_COMPILER="%MINGW_BIN%\g++.exe"
if errorlevel 1 exit /b 1

echo.
echo ========================================
echo Building Release...
echo ========================================
cmake --build "%BUILD_DIR%" -j
if errorlevel 1 exit /b 1

set "EXE_PATH=%BUILD_DIR%\%APP_NAME%.exe"
if not exist "%EXE_PATH%" (
    for /r "%BUILD_DIR%" %%F in (*.exe) do (
        if /I "%%~nxF"=="%APP_NAME%.exe" set "EXE_PATH=%%~fF"
    )
)

if not exist "%EXE_PATH%" (
    echo ERROR: Built executable not found.
    exit /b 1
)

echo Built executable:
echo   %EXE_PATH%

if exist "%QT_PREFIX%\bin\windeployqt.exe" (
    echo.
    echo ========================================
    echo Deploying Qt runtime...
    echo ========================================
    "%QT_PREFIX%\bin\windeployqt.exe" "%EXE_PATH%"
)

echo.
echo ========================================
echo Launching application...
echo ========================================
start "" "%EXE_PATH%"
exit /b 0
