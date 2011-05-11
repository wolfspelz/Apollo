// ---------------------------------------------------
// Utils.js
//
// A combination of several includes

// ---------------------------------------------------
// Funktions.js

Function.prototype.bind = function(fn)
{
  var m = this;
  return function() {
    return m.apply(fn, arguments);
  }
}

String.prototype.startsWith = function(s)
{
  return (this.match('^' + s) == s);
}

String.prototype.toDomCompatible = function()
{
	var s = '';
	var sValue = this;
  for (var i = 0; i < sValue.length; i++) {
    var c = sValue.charAt(i);
    if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9') {
		  s += c;
    } else if (c == '_') {
		  s += '__';
    } else {
		  s += '_' + sValue.charCodeAt(i);
    }
	}	
	return s;
}

//var String_Tester = {
//  toDomCompatible: function() {
//    var s = 'a1_2@b-c.d';
//    var t = s.toDomCompatible();
//    Log.Debug(t);
//  },
//}

Object.extend = function(baseClass, derivedClass)
{
  derivedClass.prototype = {};
  for (var i in baseClass.prototype) {
    derivedClass.prototype[i] = baseClass.prototype[i];
    derivedClass.prototype.baseClass = baseClass;
  }
}

// ---------------------------------------------------
// Log.js

function Logger(sDestinationDomId, sGateCheckboxDomId)
{
  this.nLevel = Log.Level.WARN;
  this.sDestinationDomId = sDestinationDomId;
  this.sGateCheckboxDomId = sGateCheckboxDomId;
}

Logger.prototype.Out = function(nLevel, sText)
{
  if (nLevel > this.nLevel) { return }
  
  var eLogCheck = document.getElementById(this.sGateCheckboxDomId);
  if (eLogCheck.checked) {
    $('#' + this.sDestinationDomId).append('<p>' + Log.Prefix[nLevel] + ' ' + sText.replace(/</ig, '&lt;') + '</p>');
    var eOut = document.getElementById(this.sDestinationDomId);
    if (eOut) {
//      eOut.value = eOut.value + Log.Prefix[nLevel] + ' ' + sText + '\n';
      eOut.scrollTop = eOut.scrollHeight;
    }
  }
}

Logger.prototype.SetLevel = function(nLevel)
{
  var nOldLevel = this.nLevel;
  this.nLevel = nLevel;
  return nOldLevel;
}

Logger.prototype.Clear = function()
{
  $('#' + this.sDestinationDomId).html('');
}

var Log = {
  instance: null,
  
  Level: {
    NONE: 0,
    FATAL: 1, // can not continue
    ERROR: 2, // bad enough
    WARNING: 3, // not good
    USERERROR: 4, // user mistakes
    DEBUG: 5, // use only for debugging, delete when not used
    INFO: 6, // few per sec
    IO: 7, // network I/O
    TRACE: 8, // (many) method calls
    VERBOSE: 9, // 100 per sec
  },
  
  Prefix: [
    '',
    'FATAL',
    'ERROR',
    'WARN.',
    'USER.',
    '#####',
    'INFO.',
    'IO...',
    'TRACE',
    '.....',
  ],
  
  Fatal: function(sText) { Log.instance.Out(Log.Level.FATAL, sText); },
  Error: function(sText) { Log.instance.Out(Log.Level.ERROR, sText); },
  Warning: function(sText) { Log.instance.Out(Log.Level.WARNING, sText); },
  UserError: function(sText) { Log.instance.Out(Log.Level.USERERROR, sText); },
  Debug: function(sText) { Log.instance.Out(Log.Level.DEBUG, sText); },
  Info: function(sText) { Log.instance.Out(Log.Level.INFO, sText); },
  IO: function(sText) { Log.instance.Out(Log.Level.IO, sText); },
  Trace: function(sText) { Log.instance.Out(Log.Level.TRACE, sText); },
  Verbose: function(sText) { Log.instance.Out(Log.Level.VERBOSE, sText); },

  GetInstance: function(sDestinationDomId, sGateCheckboxDomId)
  {
    if (Log.instance == null)  {
      Log.instance = new Logger(sDestinationDomId, sGateCheckboxDomId);
    }
    return Log.instance;
  },
}

// ---------------------------------------------------
// Unittest.js

Unittest.SuccessTest = function() { return ''; }
Unittest.FailedTest= function() { return 'error'; }
Unittest.ExceptionTest = function() { throw new Object(); }

function Unittest()
{
  this.tests = new Object();
  this.nTotal = 0;
  this.nSuccess = 0;
  this.nFailed = 0;
  this.nComplete = 0;
  this.bSuccess = false;
}

Unittest.prototype.Add = function(sName, fTest)
{
  this.tests[sName] = fTest;
}

Unittest.prototype.Run = function()
{
  this.nSuccess = 0;
  this.nFailed = 0;
  this.nComplete = 0;
  this.bSuccess = false;

  var nPreviousLevel = Log.instance.SetLevel(Log.Level.DEBUG);

  for (var sName in this.tests) {
    var f = this.tests[sName];
    var s = '';
    try {
      s = f(this);
    } catch (ex) {
      s = 'Exception:' + ex.message + ' in ' + ex.fileName + '(' + ex.lineNumber + ')';
    }
    this.nComplete++;
    if (s == null || s == '') {
      this.nSuccess++;
      Log.Debug(sName);
    } else {
      this.nFailed++;
      Log.Debug('>>>>>> ' + sName + ': ' + s);
    }
  }
  
  if (this.nTotal == this.nSuccess) {
    this.bSuccess = true;
  }

  Log.instance.SetLevel(nPreviousLevel);
  Log.Debug('-----> total=' + this.nTotal + ' complete=' + this.nComplete + ' success=' + this.nSuccess + ' failed=' + this.nFailed + ' ');
}

// ---------------------------------------------------------
// Observable.js

function Observable()
{
  this.observers = new Object();
}

Observable.prototype.Add = function(sName, fObserver)
{
  this.observers[sName] = fObserver;
}

Observable.prototype.Remove = function(sName)
{
  delete this.observers[sName];
}

Observable.prototype.Notify = function(msg)
{
  var sListeners = '(';
  var bFirst = true;
  for (var sName in this.observers) {
    if (!bFirst) { sListeners += ','; }
    bFirst = false;
    sListeners += sName;
  }
  sListeners += ')';
  Log.Verbose('Observable.Notify ' + sListeners + ' ' + $.param(msg));

  for (var sName in this.observers) {
    try {
      this.observers[sName](msg);
    } catch (ex) {
      Log.Error('Observable.Notify ' + sName + ' ' + ex.message);
    }
  }
}

var Observable_Tester = {
  nCnt: 0,

  Listener1: function(msg) { Observable_Tester.nCnt += msg.c; },
  Listener2: function(msg) { Observable_Tester.nCnt += msg.c; },

  Basic: function() {
    var o = new Observable();
    o.Add('1', Observable_Tester.Listener1);
    o.Add('2', Observable_Tester.Listener2);
    Observable_Tester.nCnt = 0;
    o.Notify({ 'a': 'b', 'c': 1});
    if (Observable_Tester.nCnt != 2) {
      return 'Expected 2 notifications, got ' + Observable_Tester.nCnt;
    }
  },

  Remove: function() {      
    var o = new Observable();
    o.Add('1', Observable_Tester.Listener1);
    o.Add('2', Observable_Tester.Listener2);
    o.Remove('1');
    Observable_Tester.nCnt = 0;
    o.Notify({ 'a': 'b', 'c': 1});
    if (Observable_Tester.nCnt != 1) {
      return 'Expected 1 notification, got ' + Observable_Tester.nCnt;
    }
  },

  Serialize: function() {      
    var msg = { 'a': 'b', 'c': 1};
    var sMsg = $.param(msg);
    if (sMsg !=  'a=b&c=1') {
      return '$.param(msg) returned "' + sMsg + '"';
    }
  },
}

