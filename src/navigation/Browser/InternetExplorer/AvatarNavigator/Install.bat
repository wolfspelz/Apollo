@echo off
if exist "AvatarNavigator.dll" "C:\Windows\Microsoft.NET\Framework\v2.0.50727\regasm" "AvatarNavigator.dll" GOTO :END
if exist "bin\Release\AvatarNavigator.dll" "C:\Windows\Microsoft.NET\Framework\v2.0.50727\regasm" "bin\Release\AvatarNavigator.dll" GOTO :END
if exist "bin\Debug\AvatarNavigator.dll" "C:\Windows\Microsoft.NET\Framework\v2.0.50727\regasm" "bin\Debug\AvatarNavigator.dll" GOTO :END
:END