@ECHO OFF

SET VERSION=latest
rem SET VERSION=3.0.3
rem SET RC_VERSION=1

PATH=%PATH%;"%CD%\bin"

SET BASE_DIR="%CD%"
SET DEBUG_SRC_DIR="%CD%\..\bin\Win32\Debug"
SET RELEASE_SRC_DIR="%CD%\..\bin\Win32\Release"
SET RELEASE_DEST_DIR="%CD%\package"
SET BUILD_NAME=apollo-win-%VERSION%
SET RELEASE_DIR=%RELEASE_DEST_DIR%\%BUILD_NAME%
SET ARCHIVE_PATH=%RELEASE_DEST_DIR%
SET ARCHIVE_NAME=%BUILD_NAME%.zip

ECHO ---------------------------------------------------------------------------
ECHO Cleaning up
rmdir %RELEASE_DIR%\ /s /q
del %ARCHIVE_PATH%\%ARCHIVE_NAME%

ECHO ---------------------------------------------------------------------------
ECHO Updating release with debug files
xcopy %DEBUG_SRC_DIR% %RELEASE_SRC_DIR% /E /H /R /Y /EXCLUDE:ReleaseUpdateExcludes.txt

ECHO ---------------------------------------------------------------------------
ECHO Copying release files
xcopy %RELEASE_SRC_DIR% %RELEASE_DIR%\ /E /H /R /Y /EXCLUDE:ReleaseExcludes.txt

ECHO ---------------------------------------------------------------------------
ECHO Zipping release files
cd %RELEASE_DIR%
7za a -y -tzip %ARCHIVE_PATH%\%ARCHIVE_NAME% *
cd %BASE_DIR%

ECHO ---------------------------------------------------------------------------
ECHO Cleaning up
rmdir %RELEASE_DIR%\ /s /q

ECHO ---------------------------------------------------------------------------
ECHO Done ...
@ECHO ON

cmd
