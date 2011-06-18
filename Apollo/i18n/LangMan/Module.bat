@echo off
REM Run LangMan for a module. Then copy files Win32/Debug and Win32/Release folders
@echo on

java -jar LangMan.jar -v -p %1 -l en -l de
xcopy /F /Y out\%1\*.txt ..\..\bin\Win32\Debug\modules\%1\

@echo off
REM Release files will be copied by BuildPackage.bat
REM xcopy /F /Y out\%1\*.txt ..\..\bin\Win32\Release\modules\%1\
