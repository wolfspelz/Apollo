
// ------------------------------------------

function SrpcMessage()
{
	this.aParams_ = new Array();
}

// ---------------------
// Setter

SrpcMessage.prototype.fromString = function(sMsg)
{
	var aLines = sMsg.split("\n");
	if (aLines.length > 0) {
    for (var i = 0; i < aLines.length; i++) {
      var nIndex = aLines[i].indexOf('=');
			if (nIndex > 0) {
				var sKey = aLines[i].substr(0, nIndex);
				var sValue = aLines[i].substr(nIndex + 1);
				if (sKey != '') {
					this.aParams_[sKey] = sValue;
				}
			}
		}
	}
}

SrpcMessage.prototype.encodeCString = function(sValue)
{
	var sResult = '';
	for (var i = 0; i < sValue.length; i++) {
		switch (sValue.charAt(i)) {
			case '\\': sResult += '\\\\'; break;
			case '\r': sResult += '\\r'; break;
			case '\n': sResult += '\\n'; break;
			default: sResult += sValue.charAt(i);
		}
	}
	return sResult;
}

SrpcMessage.prototype.setString = function(sKey, sValue)
{
	sValue = this.encodeCString(sValue);
	this.aParams_[sKey] = sValue;
}

SrpcMessage.prototype.setInt = function(sKey, nValue)
{
	this.aParams_[sKey] = String(nValue);
}

SrpcMessage.prototype.setKeyValueList = function(sKey, kvList)
{
  var s = '';
  for (var i in kvList) { 
    s += i + '=' + this.encodeCString(kvList[i]) + '\n';
  }
	this.setString(sKey, s);
}

// ---------------------
// Getter

SrpcMessage.prototype.getList = function()
{
	return this.aParams_;
}

SrpcMessage.prototype.decodeCString = function(sValue)
{
	var sResult = '';
	var bEscape = false;
	for (var i = 0; i < sValue.length; i++) {
		var sChar = sValue.charAt(i);
		if (!bEscape) {
			if (sChar == '\\') {
				bEscape = true;
			} else {
				sResult += sChar;
			}
		} else {
			switch (sChar) {
				case '\\': sChar = '\\'; break;
				case 'r': sChar = '\r'; break;
				case 'n': sChar = '\n'; break;
				default:;
			}
			sResult += sChar;
			bEscape = false;
		}
	}
	return sResult;
}

SrpcMessage.prototype.getString = function(sKey)
{
	var sValue = '';
	if (this.aParams_[sKey]) {
		sValue = this.aParams_[sKey];
		//var sEncoding = this.aParams_[sKey + '/Encoding'];
		//if (sEncoding == 'cstring') {
			sValue = this.decodeCString(sValue);
		//}
	}
	return sValue;
}

SrpcMessage.prototype.getInt = function(sKey)
{
	return parseInt(this.getString(sKey));
}

SrpcMessage.prototype.getKeyValueList = function(sKey)
{
  var kvList = new Object();
  var srpc = new SrpcMessage();
  srpc.fromString(this.getString(sKey));
  for (var i in srpc.getList()) {
    kvList[i] = srpc.getList()[i];
  }
	return kvList;
}

// ---------------------
// IO

SrpcMessage.prototype.aRequests_ = new Array();
SrpcMessage.prototype.nSrpcId_ = 0;

SrpcMessage.prototype.send = function()
{
  if (window.apollo) {
    var sMsg = '';
    for (var i in this.getList()) { sMsg += i + '=' + this.getList()[i] + '\n'; }
    if (typeof(onLog) == 'function') { onLog(sMsg); }

    window.apollo.sendMessage(this.getList());
  }
}

SrpcMessage.prototype.request = function(fnCallback)
{
  var nSrpcId = SrpcMessage.prototype.nSrpcId_++;
  this.setInt('SrpcId', nSrpcId);
  SrpcMessage.prototype.aRequests_[nSrpcId] = {'sMethod': this.getString('Method'), 'fnCallback': fnCallback};

  this.send();
}

// ---------------------
// Unittest

SrpcMessage.prototype.unitTest = function()
{
	var s = '';

	if (!s) {
		var msg = new SrpcMessage();
		msg.fromString('Method=Test\na=b\nc=d\n');
		if (!s) { if (msg.getList()['Method'] != 'Test') { s = 'Missing Method'; } }
		if (!s) { if (msg.getList()['a'] != 'b') { s = 'Missing a=b'; } }
		if (!s) { if (msg.getList()['c'] != 'd') { s = 'Missing c=d'; } }
	}

	if (!s) {
		var msg = new SrpcMessage();
		msg.fromString('Method=Test\na=w=x\\ny=z\\n\nc=d\n');
		if (!s) { if (msg.getList()['Method'] != 'Test') { s = 'Missing Method'; } }
		if (!s) { if (msg.getList()['a'] != 'w=x\\ny=z\\n') { s = 'Missing a=w=x\\ny=z\\n'; } }
		if (!s) { if (msg.getList()['c'] != 'd') { s = 'Missing c=d'; } }
	}

	return s;
}

// ------------------------------------------

ApMessage.prototype = new SrpcMessage;

function ApMessage(sType)
{
	this.inheritFrom = SrpcMessage;
  this.inheritFrom();

	if (sType != null) {
		this.aParams_['Method'] = 'Host.Call';
		this.aParams_['ApType'] = sType;
	}
}

// ------------------------------------------

function onRecvMessage(s)
{
  var msg = new SrpcMessage();
  msg.fromString(s);
  var sMsg = '';
  for (var i in msg.getList()) { sMsg += '    ' + i + '=' + msg.getList()[i] + '\n'; }
  if (typeof(onLog) == 'function') { onLog(sMsg); }

  var bOnMessage = true;

  if (msg.getString('SrpcId') != '') {
    var data = SrpcMessage.prototype.aRequests_[msg.getString('SrpcId')];
    if (data) {
      if (data.fnCallback) {
        data.fnCallback(msg);
        bOnMessage = false;
      }
      SrpcMessage.prototype.aRequests_[msg.getString('SrpcId')] = null;
    }
  }

  if (bOnMessage) {
    if (typeof(onMessage) == 'function') { onMessage(msg); }
  }
}

function onRecvConfigResponse(sValue, sRef)
{
  onConfigResponse(sValue, sRef);
}

var bridge = {};
bridge.recvMessage = onRecvMessage;
bridge.recvConfigResponse = onRecvConfigResponse;
window.childSandboxBridge = bridge;
window.apollo = window.parentSandboxBridge;

// ---------------------

function onLoad(e)
{
	var msg = new SrpcMessage();
	msg.setString('Method', 'Display.Loaded');
	msg.send();

	window.removeEventListener('load', onLoad, false);
}

function onUnload(e)
{
	var msg = new SrpcMessage();
	msg.setString('Method', 'Display.BeforeUnload');
	msg.send();

	window.removeEventListener('unload', onUnload, false);
}

window.addEventListener('load', onLoad, false);
window.addEventListener('unload', onUnload, false);

