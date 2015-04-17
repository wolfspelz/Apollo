@echo OFF

PATH=%PATH%;%CD%\bin

REM chrome.exe path for packing extensions
PATH=%PATH%;C:\Users\wolf\AppData\Local\Google\Chrome\Application

set PRODUCT_NAME=AvatarNavigator
set DEST_FILE=%PRODUCT_NAME%.crx

set WORK_DIR=%CD%
set BASE_DIR=%WORK_DIR%\..
set SRC_DIR=%BASE_DIR%\src\navigation\Browser\Chrome
set BUILD_DIR=%WORK_DIR%\build
set DEST_DIR=%BASE_DIR%\bin\Win32\Debug\modules\navigation

echo ---------------------------------------------------------------------------
echo Cleaning up
del %BUILD_DIR%\Chrome\%DEST_FILE%
rmdir %BUILD_DIR%\Chrome\%PRODUCT_NAME% /s /q

echo ---------------------------------------------------------------------------
echo Copying release files
mkdir %BUILD_DIR%\Chrome\%PRODUCT_NAME%
xcopy %SRC_DIR%\%PRODUCT_NAME% %BUILD_DIR%\Chrome\%PRODUCT_NAME%\ /E /H /R /Y /EXCLUDE:%SRC_DIR%\ReleaseExcludes.txt

echo ---------------------------------------------------------------------------
echo Pack extension
chrome.exe --pack-extension=%BUILD_DIR%\Chrome\%PRODUCT_NAME% --pack-extension-key=%SRC_DIR%\AvatarNavigator.pem --no-message-box

echo ---------------------------------------------------------------------------
echo Copying result to destination
xcopy %BUILD_DIR%\Chrome\%DEST_FILE% %DEST_DIR%\ /R /Y

pause