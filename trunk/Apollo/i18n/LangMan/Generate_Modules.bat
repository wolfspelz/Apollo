java -jar LangMan.jar -v -p Arena -l en -l de
xcopy /F /Y out\Arena\*.txt ..\..\bin\Win32\Debug\modules\Arena\
xcopy /F /Y out\Arena\*.txt ..\..\bin\Win32\Release\modules\Arena\

java -jar LangMan.jar -v -p Chat -l en -l de
xcopy /F /Y out\Arena\*.txt ..\..\bin\Win32\Debug\modules\Chat\
xcopy /F /Y out\Arena\*.txt ..\..\bin\Win32\Release\modules\Chat\

pause