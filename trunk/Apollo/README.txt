General
-------

It should just compile.
If you run it, then it will register a new account, start the xmpp client and log in. 
Check the console window for XMPP I/O.
Only the console window will show up. 
No other user interfaces, yet.


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