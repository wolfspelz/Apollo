@echo off
if exist "AvatarNavigator.dll" "C:\Windows\Microsoft.NET\Framework\v2.0.50727\regasm" /unregister "AvatarNavigator.dll"
if exist "bin\Debug\AvatarNavigator.dll" "C:\Windows\Microsoft.NET\Framework\v2.0.50727\regasm" /unregister "bin\Debug\AvatarNavigator.dll"
if exist "bin\Release\AvatarNavigator.dll" "C:\Windows\Microsoft.NET\Framework\v2.0.50727\regasm" /unregister "bin\Release\AvatarNavigator.dll"