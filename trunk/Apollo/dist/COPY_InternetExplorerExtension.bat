@echo OFF

PATH=%PATH%;%CD%\bin

set PRODUCT_NAME=AvatarNavigator
set DEST_FILE=%PRODUCT_NAME%.msi

set WORK_DIR=%CD%
set BASE_DIR=%WORK_DIR%\..
set SRC_DIR=%BASE_DIR%\src\navigation\Browser\InternetExplorer\Setup\Release
set DEST_DIR=%BASE_DIR%\bin\Win32\Debug\modules\navigation

echo ---------------------------------------------------------------------------
echo Copying result to Debug destination
xcopy %SRC_DIR%\%DEST_FILE% %DEST_DIR%\ /R /Y

pause