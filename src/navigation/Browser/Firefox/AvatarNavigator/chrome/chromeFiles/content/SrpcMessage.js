
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
