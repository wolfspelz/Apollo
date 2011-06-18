REM Run LangMan for a module. Then copy files Win32/Debug and Win32/Release folders

java -jar LangMan.jar -v -p %1 -l en -l de
xcopy /F /Y out\%1\*.txt ..\..\bin\Win32\Debug\modules\%1\
xcopy /F /Y out\%1\*.txt ..\..\bin\Win32\Release\modules\%1\
