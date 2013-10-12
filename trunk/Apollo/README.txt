
THS ÌS INFORMATION IS OUT OF DATE
IF YOU NEED INSRUCTIONS, THEN MAIL ME AND I WILL BE HAPPY TO PRODUCE THEM
I AM JUST TOO LAZY TO WRITE DOWN INSTRUCTIONS IF NOBODY NEEDS THEM, SO TELL ME

General
-------

This is a MSVC 10 (2010) project.

Set Win32App as startup project. If you run it, then it will register a new account, start the xmpp client and log in. 

QT
--

To compile QT projects, set %QTDIR% to your QT folder.

See also:
http://truth2.wordpress.com/2010/09/04/custom-build-rules-for-qt-under-visual-studio-2010/
http://qt-project.org/forums/viewthread/20222/

Starting Development
--------------------

Module config files contain available parameters:
  bin/Debug/modules/<MODULE>/config.txt

They can be added to a local config file:
  bin/Debug/config.local.txt
A template is available as:
  bin/Debug/_config.local.txt


VisualLeakDetector 
------------------

For the Debug version to use VisualLeakDetector, you will need Debugging Tools for Windows (about 17MB) from:
  http://www.microsoft.com/whdc/DevTools/Debugging/ 

Add inc and lib path to your MSVC settings:
  Tools/Options/ "Projects and Solutions"/"VC++ Directories"
Include Files:
  C:\{Program Files}\Debugging Tools for Windows (x86)\sdk\inc
Library Files:
  C:\{Program Files}\Debugging Tools for Windows (x86)\sdk\lib\i386


Best
hw