REM For all modules
FOR %%i IN (Arena,Chat,Setup) DO CALL Module.bat %%i 

REM AvatarTray is not a module
java -jar LangMan.jar -v -p AvatarTray -l en -l de -l ja
rename LangMan.log AvatarTray.log
xcopy /F /Y out\AvatarTray\*.txt ..\..\bin\Win32\Debug\

pause