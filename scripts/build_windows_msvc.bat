@echo off
setlocal
pushd "%~dp0\.."
call build_release_and_run.bat %*
set EXIT_CODE=%ERRORLEVEL%
popd
exit /b %EXIT_CODE%
