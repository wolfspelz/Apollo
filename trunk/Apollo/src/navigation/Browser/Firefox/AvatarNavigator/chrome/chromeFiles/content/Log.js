
var anLogLevelError = 1;
var anLogLevelWarning = 2;
var anLogLevelInfo = 3;
var anLogLevelDebug = 4;
var anLogLevelVerbose = 5;
var anLogLevelTrace = 6;

// console service for logging
var anLogConsoleSvc = Components.classes['@mozilla.org/consoleservice;1'].getService(Components.interfaces.nsIConsoleService);
var anLogPrefManager = Components.classes["@mozilla.org/preferences-service;1"].getService(Components.interfaces.nsIPrefBranch);

var anLogLevel = anLogLevelInfo; try { anLogLevel = anLogPrefManager.getIntPref('extensions.avatarnav.loglevel'); } catch (ex) {}

function anLog(nLevel, sText)
{
  if (anLogLevel >= nLevel) {
    var sLevel = '???.....';
    switch (nLevel) {
      case anLogLevelError:    { sLevel = 'ERROR...'; } break;
      case anLogLevelWarning:  { sLevel = 'WARNING.'; } break;
      case anLogLevelInfo:     { sLevel = 'INFO....'; } break;
      case anLogLevelDebug:    { sLevel = 'DEBUG...'; } break;
      case anLogLevelVerbose:  { sLevel = 'VERBOSE.'; } break;
      case anLogLevelTrace:    { sLevel = 'TRACE...'; } break;
    }
    anLogConsoleSvc.logStringMessage('Avatar ' + sLevel + ' ' + sText.replace(/[\\]/g, "\\\\").replace(/(\r\n|[\r\n])/g, "\\n"));
  }
}
function anLogError(sText) { anLog(anLogLevelError, sText); }
function anLogWarning(sText) { anLog(anLogLevelWarning, sText); }
function anLogInfo(sText) { anLog(anLogLevelInfo, sText); }
function anLogDebug(sText) { anLog(anLogLevelDebug, sText); }
function anLogVerbose(sText) { anLog(anLogLevelVerbose, sText); }
function anLogTrace(sText) { anLog(anLogLevelTrace, sText); }
