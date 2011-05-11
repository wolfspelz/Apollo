// ------------------------------------------
// ApMessage

function ApMessage(sMethod)
{
	this.aParams_ = new Array();
  if (sMethod != null) {
    this.setString('Method', sMethod);
  }
}

ApMessage.prototype = 
{
  // ---------------------
  // Setter

  fromString: function(sMsg)
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
  },

  encodeCString: function(sValue)
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
  },

  setString: function(sKey, sValue)
  {
	  sValue = this.encodeCString(sValue);
	  this.aParams_[sKey] = sValue;
	  return this;
  },

  setInt: function(sKey, nValue)
  {
	  this.aParams_[sKey] = String(nValue);
	  return this;
  },

  setKeyValueList: function(sKey, kvList)
  {
    var s = '';
    for (var i in kvList) { 
      s += i + '=' + this.encodeCString(kvList[i]) + '\n';
    }
	  this.setString(sKey, s);
	  return this;
  },

  // ---------------------
  // Getter

  decodeCString: function(sValue)
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
  },

  getString: function(sKey)
  {
	  var sValue = '';
	  if (this.aParams_[sKey]) {
		  sValue = this.aParams_[sKey];
		  sValue = this.decodeCString(sValue);
	  }
	  return sValue;
  },

  getInt: function(sKey)
  {
	  return parseInt(this.getString(sKey));
  },

  getKeyValueList: function(sKey)
  {
    var kvList = new Object();
    var srpc = new ApMessage();
    srpc.fromString(this.getString(sKey));
    for (var i in srpc.aParams_) {
      kvList[i] = srpc.aParams_[i];
    }
	  return kvList;
  },

  // ---------------------
  // IO

  send: function()
  {
    if (apollo) {
      var sMsg = '';
      for (var i in this.aParams_) { sMsg += i + '=' + this.aParams_[i] + '\n'; }
      if (typeof(onLog) == 'function') { onLog(sMsg); }

      var sRespose = apollo.sendMessage(sMsg);
      
      var resp = new ApMessage();
      resp.fromString(sRespose);
      
      return resp;
    }
  },

  // ---------------------
  // Test

  unitTest: function()
  {
	  var s = '';

	  if (!s) {
		  var msg = new ApMessage();
		  msg.fromString('Method=Test\na=b\nc=d\n');
		  if (!s) { if (msg.aParams_['Method'] != 'Test') { s = 'Missing Method'; } }
		  if (!s) { if (msg.aParams_['a'] != 'b') { s = 'Missing a=b'; } }
		  if (!s) { if (msg.aParams_['c'] != 'd') { s = 'Missing c=d'; } }
	  }

	  if (!s) {
		  var msg = new ApMessage();
		  msg.fromString('Method=Test\na=w=x\\ny=z\\n\nc=d\n');
		  if (!s) { if (msg.aParams_['Method'] != 'Test') { s = 'Missing Method'; } }
		  if (!s) { if (msg.aParams_['a'] != 'w=x\\ny=z\\n') { s = 'Missing a=w=x\\ny=z\\n'; } }
		  if (!s) { if (msg.aParams_['c'] != 'd') { s = 'Missing c=d'; } }
	  }

	  return s;
  },
  
  _:0
}

// ------------------------------------------
// ApolloInterface

Function.prototype.bind = function(fn)
{
  var m = this;
  return function() {
    return m.apply(fn, arguments);
  }
}

function ApolloApi() {}

ApolloApi.prototype = 
{
  moduleName: 'Unknown',
  
  // ------------------------------------------------
  // SRPC
  
  msg: function(sType)
  {
    return new ApMessage(sType);
  },

  // ------------------------------------------------
  // Log
  
  log: function(nMask, sChannel, sContext, sMessage)
  {
    return this.msg('Log_Line')
      .setInt('nMask', nMask)
      .setString('sChannel', sChannel)
      .setString('sContext', sContext)
      .setString('sMessage', sMessage)
      .send();
  },

  LogLevelFatal      :   2,
  LogLevelError      :   4,
  LogLevelWarning    :   8,
  LogLevelUser       :  16,
  LogLevelDebug      :  32,
  LogLevelInfo       :  64,
  LogLevelVerbose    : 128,

  logUser: function(sMessage) { this.log(this.LogLevelUser, '', '', sMessage); },
  logInfo: function(sContext, sMessage) { this.log(this.LogLevelInfo, this.moduleName, sContext, sMessage); },
  logWarning: function(sContext, sMessage) { this.log(this.LogLevelWarning, this.moduleName, sContext, sMessage); },

  // ------------------------------------------------
  // Babelfish
  
  translate: function(sContext, sText)
  {
    if (sText == null) {
      sText = sContext;
      sContext = '';
    }
    
    return this.msg('Translation_Get')
      .setString('sModule', this.moduleName)
      .setString('sContext', sContext)
      .setString('sText', sText)
      .send()
      .getString('sTranslated');
  },

  // ------------------------------------------------
  // Move and size
  
  nStartX: 0,
  nStartY: 0,
  bMouseActive: false,
  bMouseCaptured: false,
  bIsMove: false,
  bIsSize: false,
  nSizeDirection: 8,
  nMinWidth: 100,
  nMinHeight: 100,
  
  onMouseDown: function(ev)
  {
    this.msg('WebView_MouseCapture').setString('hView', apollo.viewHandle).send();
    this.nStartX = ev.x;
    this.nStartY = ev.y;
    this.bMouseActive = false;
    this.bMouseCaptured = true;

    if (ev.target.className =='Move') {
      this.bIsMove = true; 
      this.bIsSize = false; 
    }
    if (ev.target.className =='SizeBottomRight') {
      this.bIsMove = false; 
      this.bIsSize = true; 
      this.nSizeDirection = 8; 
    }
  },

  onMouseMove: function(ev)
  {
    if (this.bMouseCaptured) {
      var dx = ev.x - this.nStartX;
      var dy = ev.y - this.nStartY;
      if (!this.bMouseActive) {
        if (dx > 4 || dx < -4 || dy > 4 || dy < -4) {
          this.bMouseActive = true;
        }
      }
      if (this.bMouseActive) {
        if (this.bIsMove) {
          this.msg('WebView_MoveBy').setString('hView', apollo.viewHandle).setInt('nX', dx).setInt('nY', dy).send();
        }
        if (this.bIsSize) {
          if (dx < 0 || dy < 0) {
            var pos = this.msg('WebView_GetPosition').setString('hView', apollo.viewHandle).send();
            var nW = pos.getInt('nW');
            var nH = pos.getInt('nH');
            var nNewW = nW + dx;
            var nNewH = nH + dy;
            if (nNewW < this.nMinWidth) { dx = this.nMinWidth - nW; }
            if (nNewH < this.nMinHeight) { dy = this.nMinHeight - nH; }
          }

          this.msg('WebView_SizeBy')
            .setString('hView', apollo.viewHandle)
            .setInt('nW', dx)
            .setInt('nH', dy)
            .setInt('nDirection', this.nSizeDirection)
            .send();

          this.nStartX += dx;
          this.nStartY += dy;
        }
      }
    }
  },

  onMouseUp: function()
  {
    this.bMouseCaptured = false;
    this.bMouseActive = false;
    this.bIsMove = false;
    this.bIsSize = false;
    this.msg('WebView_MouseRelease').setString('hView', apollo.viewHandle);
  },

  initMoveSize: function()
  {
    document.onmousedown = this.onMouseDown.bind(this); 
    document.onmousemove = this.onMouseMove.bind(this); 
    document.onmouseup = this.onMouseUp.bind(this);
  },

  // ------------------------------------------------
  
  init: function()
  {
  },

  _:0
}
