
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
	
	if (typeof(sValue) == 'string') {
	  for (var i = 0; i < sValue.length; i++) {
		  switch (sValue.charAt(i)) {
			  case '\\': sResult += '\\\\'; break;
			  case '\r': sResult += '\\r'; break;
			  case '\n': sResult += '\\n'; break;
			  default: sResult += sValue.charAt(i);
		  }
	  }
	} else {
	  sResult = '' + sValue;
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

SrpcMessage.prototype.setValueList = function(sKey, aList)
{
  var s = '';
  for (var i = 0; i < aList.length; i++) {
    if (s != '') { s += ' '; }
    s += this.encodeCString(aList[i]);
  }
	this.setString(sKey, s);
}

SrpcMessage.prototype.setKeyValueList = function(sKey, oList)
{
  var s = '';
  for (var i in oList) {
    s += i + '=' + this.encodeCString(oList[i]) + '\n';
  }
	this.setString(sKey, s);
}

// ---------------------
// Getter

SrpcMessage.prototype.toString = function()
{
  var s = '';
  for (var i in this.aParams_) {
    s += i + '=' + this.aParams_[i] + '\n';
  }
  return s;
}

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

SrpcMessage.prototype.getValueList = function(sKey)
{
  var aList = new Array();
  var sValue = this.getString(sKey);
  var aValue = sValue.split(' ');
	return aValue;
}

SrpcMessage.prototype.getKeyValueList = function(sKey)
{
  var oList = new Object();
  var srpc = new SrpcMessage();
  srpc.fromString(this.getString(sKey));
  for (var i in srpc.getList()) {
    oList[i] = srpc.getList()[i];
  }
	return oList;
}

// ---------------------
// IO

SrpcMessage.prototype.aRequests_ = new Array();
SrpcMessage.prototype.nSrpcId_ = 0;

SrpcMessage.prototype.send = function()
{
  if (window.apollo) {
    if (typeof(onLog) == 'function') { 
      var sMsg = this.toString();
      onLog(sMsg);
    }

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

	if (!s) {
		var msg = new SrpcMessage();
		var kvList = new Object();
		kvList['a'] = 'b';
		kvList['c'] = 'd\ne';
		kvList['f'] = 111;
		msg.setKeyValueList('kvList', kvList);
		var sExpectedValue = 'a=b\\nc=d\\\\ne\\nf=111\\n';
		var sExpected = 'kvList='+sExpectedValue+'\n';
		if (!s) { if (msg.toString() != sExpected) { s = 'setKeyValueList failed: ['+msg.toString()+'] != ['+sExpected+']'; } }
		var kvList2 = msg.getKeyValueList('kvList');
		if (!s) { if (kvList2['a'] != 'b') { s = 'getKeyValueList a wrong'; }} 
		if (!s) { if (kvList2['c'] != 'd\\ne') { s = 'getKeyValueList c wrong'; }} 
		if (!s) { if (kvList2['f'] != '111') { s = 'getKeyValueList f wrong'; }} 
	}

	if (!s) {
		var msg = new SrpcMessage();
		var vlList = new Array();
		vlList.push('a');
		vlList.push('b');
		vlList.push(111);
		msg.setValueList('vlList', vlList);
		var sExpectedValue = 'a b 111';
		var sExpected = 'vlList='+sExpectedValue+'\n';
		if (!s) { if (msg.toString() != sExpected) { s = 'setValueList failed: ['+msg.toString()+'] != ['+sExpected+']'; } }
		var vlList2 = msg.getValueList('vlList');
		if (!s) { if (vlList2[0] != 'a') { s = 'getValueList 0 wrong'; }} 
		if (!s) { if (vlList2[1] != 'b') { s = 'getValueList 1 wrong'; }} 
		if (!s) { if (vlList2[2] != 111) { s = 'getValueList 2 wrong'; }} 
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

// Defined and used here only for received messages
SrpcMessage.prototype.toStringPrefix = function(sPrefix)
{
  var s = '';
  for (var i in this.aParams_) {
    s += sPrefix + i + '=' + this.aParams_[i] + '\n';
  }
  return s;
}

function onRecvMessage(s)
{
  var msg = new SrpcMessage();
  msg.fromString(s);
  
  if (typeof(onLog) == 'function') {
    var sMsg = msg.toStringPrefix('     ');
    onLog(sMsg);
  }

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
