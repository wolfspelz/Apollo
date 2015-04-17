
var anLogLevelError = 1;
var anLogLevelWarning = 2;
var anLogLevelInfo = 3;
var anLogLevelDebug = 4;
var anLogLevelVerbose = 5;
var anLogLevelTrace = 6;

// console service for logging
if (!window.console) console = {};
console.log = console.log || function () { };
console.warn = console.warn || function () { };
console.error = console.error || function () { };
console.info = console.info || function () { };

var anLogLevel = anLogLevelInfo; try { anLogLevel = anLogPrefManager.getIntPref('extensions.avatarnav.loglevel'); } catch (ex) {}

function anLog(nLevel, sText)
{
  if (anLogLevel >= nLevel) {
    var sLevel = '???.....';
    switch (nLevel) {
      case anLogLevelError: { sLevel = 'ERROR...'; } break;
      case anLogLevelWarning: { sLevel = 'WARNING.'; } break;
      case anLogLevelInfo: { sLevel = 'INFO....'; } break;
      case anLogLevelDebug: { sLevel = 'DEBUG...'; } break;
      case anLogLevelVerbose: { sLevel = 'VERBOSE.'; } break;
      case anLogLevelTrace: { sLevel = 'TRACE...'; } break;
    }
    switch (nLevel) {
      case anLogLevelError:
        console.error('Avatar ' + sLevel + ' ' + sText.replace(/[\\]/g, "\\\\").replace(/(\r\n|[\r\n])/g, "\\n"));
        break;
      case anLogLevelWarning:
        console.warn('Avatar ' + sLevel + ' ' + sText.replace(/[\\]/g, "\\\\").replace(/(\r\n|[\r\n])/g, "\\n"));
        break;
      case anLogLevelInfo:
        console.info('Avatar ' + sLevel + ' ' + sText.replace(/[\\]/g, "\\\\").replace(/(\r\n|[\r\n])/g, "\\n"));
        break;
      case anLogLevelDebug:
      case anLogLevelVerbose:
      case anLogLevelTrace:
        console.log('Avatar ' + sLevel + ' ' + sText.replace(/[\\]/g, "\\\\").replace(/(\r\n|[\r\n])/g, "\\n"));
        break;
    }
  }
}
function anLogError(sText) { anLog(anLogLevelError, sText); }
function anLogWarning(sText) { anLog(anLogLevelWarning, sText); }
function anLogInfo(sText) { anLog(anLogLevelInfo, sText); }
function anLogDebug(sText) { anLog(anLogLevelDebug, sText); }
function anLogVerbose(sText) { anLog(anLogLevelVerbose, sText); }
function anLogTrace(sText) { anLog(anLogLevelTrace, sText); }
