@echo OFF

set VERSION=latest
rem set VERSION=3.0.4
rem set RC_VERSION=1

PATH=%PATH%;%CD%\bin

set PRODUCT_NAME=apollo-win-%VERSION%
set DEST_FILE=%PRODUCT_NAME%.zip

set WORK_DIR=%CD%
set BASE_DIR=%WORK_DIR%\..
set DEBUG_SRC_DIR=%BASE_DIR%\bin\Win32\Debug
set RELEASE_SRC_DIR=%BASE_DIR%\bin\Win32\Release
set BUILD_DIR=%WORK_DIR%\build
set DEST_DIR=%WORK_DIR%\package

echo ---------------------------------------------------------------------------
echo Cleaning up
del %BUILD_DIR%\%DEST_FILE%
rmdir %BUILD_DIR%\%PRODUCT_NAME% /s /q

echo ---------------------------------------------------------------------------
echo Building components
rem call BUILD_FirefoxExtension.bat

echo ---------------------------------------------------------------------------
echo Updating release with debug files
xcopy %DEBUG_SRC_DIR% %RELEASE_SRC_DIR% /E /H /R /Y /EXCLUDE:%BASE_DIR%\dist\ReleaseUpdateExcludes.txt

echo ---------------------------------------------------------------------------
echo Copying release files
xcopy %RELEASE_SRC_DIR% %BUILD_DIR%\%PRODUCT_NAME%\ /E /H /R /Y /EXCLUDE:ReleaseExcludes.txt

echo ---------------------------------------------------------------------------
echo Zipping release files
cd %BUILD_DIR%\%PRODUCT_NAME%
7za a -y -tzip %BUILD_DIR%\%DEST_FILE% *
cd %WORK_DIR%

echo ---------------------------------------------------------------------------
echo Copying result to destination
xcopy %BUILD_DIR%\%DEST_FILE% %DEST_DIR%\ /R /Y

cmd
