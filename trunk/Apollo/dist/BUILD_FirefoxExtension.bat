@echo OFF

PATH=%PATH%;%CD%\bin

set PRODUCT_NAME=AvatarNavigator
set DEST_FILE=%PRODUCT_NAME%.xpi

set WORK_DIR=%CD%
set BASE_DIR=%WORK_DIR%\..
set SRC_DIR=%BASE_DIR%\src\navigation\Browser\Firefox
set BUILD_DIR=%WORK_DIR%\build
set DEST_DIR=%BASE_DIR%\bin\Win32\Debug\modules\navigation

echo ---------------------------------------------------------------------------
echo Cleaning up
del %BUILD_DIR%\Firefox\%DEST_FILE%
rmdir %BUILD_DIR%\Firefox\%PRODUCT_NAME% /s /q

echo ---------------------------------------------------------------------------
echo Copying release files
mkdir %BUILD_DIR%\Firefox\%PRODUCT_NAME%
xcopy %SRC_DIR%\%PRODUCT_NAME% %BUILD_DIR%\Firefox\%PRODUCT_NAME%\ /E /H /R /Y /EXCLUDE:%SRC_DIR%\ReleaseExcludes.txt

echo ---------------------------------------------------------------------------
echo Zipping release files
cd %BUILD_DIR%\Firefox\%PRODUCT_NAME%
7za a -y -tzip %BUILD_DIR%\Firefox\%DEST_FILE% *
cd %WORK_DIR%

echo ---------------------------------------------------------------------------
echo Copying result to destination
xcopy %BUILD_DIR%\Firefox\%DEST_FILE% %DEST_DIR%\ /R /Y

pause