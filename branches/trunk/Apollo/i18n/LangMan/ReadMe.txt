Some hints for the LangMan

1. If you want to make use of all features (export in Shift-JIS) make sure that you have a multilanguage 
   java runtime (min java 5 and including "Additional languages and media sevices"/"Zusätzliche Sprachen und Mediendienste") and support for foreign/asian languages installed.
   
2. Make sure that your templates don't contain a BOM!

3. In case you create c++ files that contain Shift-JIS (Updater) you have to escape all backslashes (not \n or \t!) that were generated with another backslash manually.

4. Do not add your local settings (LangMan.local.properties) or your logfile (by default LangMan.log) to the svn.
