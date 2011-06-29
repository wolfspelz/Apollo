REM For all modules
FOR %%i IN (Arena,Chat) DO CALL Module.bat %%i 

REM TrayIcon is not a module
java -jar LangMan.jar -v -p TrayIcon -l en -l de -l ja
rename LangMan.log TrayIcon.log
xcopy /F /Y out\TrayIcon\*.txt ..\..\bin\Win32\Debug\

pause