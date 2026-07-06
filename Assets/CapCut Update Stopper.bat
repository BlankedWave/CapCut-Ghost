@echo off
title CapCut Update Stopper

:: ==========================================
:: Created by Muhammad Aqil
:: CapCut Update Stopper
:: ==========================================

:: Request Administrator
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo Requesting Administrator privileges...
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b
)

color 0A
echo ==========================================
echo        CapCut Update Stopper
echo         By Muhammad Aqil
echo ==========================================
echo.

set "FOLDER1=%LOCALAPPDATA%\CapCut\User Data\Download"
set "FOLDER2=%LOCALAPPDATA%\CapCut\Apps"

echo ------------------------------------------
echo Processing:
echo %FOLDER1%
echo ------------------------------------------

if exist "%FOLDER1%" (
    if exist "%FOLDER1%\update.exe" (
        attrib -r "%FOLDER1%\update.exe" >nul 2>&1
        del /f /q "%FOLDER1%\update.exe" >nul 2>&1
    )

    type nul > "%FOLDER1%\update.exe"

    if exist "%FOLDER1%\update.exe" (
        attrib +r "%FOLDER1%\update.exe"
        echo [OK] Created update.exe
    ) else (
        echo [FAILED] Could not create update.exe
    )
) else (
    echo [FAILED] Folder does not exist.
)

echo.
echo ------------------------------------------
echo Processing:
echo %FOLDER2%
echo ------------------------------------------

if exist "%FOLDER2%" (
    if exist "%FOLDER2%\update.exe" (
        attrib -r "%FOLDER2%\update.exe" >nul 2>&1
        del /f /q "%FOLDER2%\update.exe" >nul 2>&1
    )

    type nul > "%FOLDER2%\update.exe"

    if exist "%FOLDER2%\update.exe" (
        attrib +r "%FOLDER2%\update.exe"
        echo [OK] Created update.exe
    ) else (
        echo [FAILED] Could not create update.exe
    )
) else (
    echo [FAILED] Folder does not exist.
)

echo.
echo ==========================================
echo Finished.
echo ==========================================
pause