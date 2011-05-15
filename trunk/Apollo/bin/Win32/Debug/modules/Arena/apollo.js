// ------------------------------------------
// ApMessage

function ApMessage(sMethod)
{
	this.aParams_ = new Array();
  if (sMethod) {
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
	  return this;
  },

  encodeCString: function(sValue)
  {
	  var sResult = '';
	  if (sValue) {
	    for (var i = 0; i < sValue.length; i++) {
		    switch (sValue.charAt(i)) {
			    case '\\': sResult += '\\\\'; break;
			    case '\r': sResult += '\\r'; break;
			    case '\n': sResult += '\\n'; break;
			    default: sResult += sValue.charAt(i);
		    }
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

  toString: function()
  {
    var sMsg = '';
    for (var i in this.aParams_) {
      sMsg += i + '=' + this.aParams_[i] + '\n';
    }
	  return sMsg;
  },

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
    if (typeof(apollo) != 'undefined') {
      var sMsg = this.toString();
      if (typeof(this.onLog) == 'function') { this.onLog('&gt; ' + sMsg); }

      var sRespose = apollo.sendMessage(sMsg);
      if (typeof(this.onLog) == 'function') { this.onLog('&nbsp;&nbsp;&lt; ' + sRespose); }
      
      return new ApMessage().fromString(sRespose);
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

function ApolloApi()
{
}

ApolloApi.prototype = 
{
  moduleName: 'Unknown',
  
  // ------------------------------------------------
  // SRPC

  OnDispatchMessage: function(msg) {},
  
  OnReceiveMessage: function (sMsg)
  {
    Log.IO('&lt; ' + sMsg);

    var msg = new ApMessage().fromString(sMsg);
    var resp = null;

    try {
      if (typeof(this.OnDispatchMessage) == 'function') {
        resp = this.OnDispatchMessage(msg);
      }
    } catch (ex) {
      resp = new ApMessage().setInt('Status', 0).setString('Message', String(ex));
    }
    
  //  if (resp == null) {
  //    resp = new ApMessage().setInt('Status', 1);
  //  }
    
    if (resp != null) {
      var sResponse = resp.toString();
      Log.IO('&nbsp;&nbsp;&gt; ' + sResponse);
      return sResponse;
    }
  },
  
  Message: function(sType)
  {
    var msg = new ApMessage(sType);
    if (sType) {
      msg.setString('hView', apollo.viewHandle);
    }
    return msg;
  },

  // ------------------------------------------------
  // Log
  
  Log: function(nMask, sChannel, sContext, sMessage)
  {
    return this.Message('Log_Line')
      .setInt('nMask', nMask)
      .setString('sChannel', sChannel)
      .setString('sContext', sContext)
      .setString('sMessage', sMessage)
      .send();
  },

  LogLevelFatal  :   2,
  LogLevelError  :   4,
  LogLevelWarning:   8,
  LogLevelUser   :  16,
  LogLevelDebug  :  32,
  LogLevelInfo   :  64,
  LogLevelVerbose: 128,

  LogError: function(sMessage) { this.Log(this.LogLevelError, this.moduleName, 'JS:', sMessage); },
  LogWarning: function(sMessage) { this.Log(this.LogLevelWarning, this.moduleName, 'JS:', sMessage); },
  LogUser: function(sMessage) { this.Log(this.LogLevelUser, this.moduleName, 'JS:', sMessage); },
  LogDebug: function(sMessage) { this.Log(this.LogLevelDebug, this.moduleName, 'JS:', sMessage); },
  LogInfo: function(sMessage) { this.Log(this.LogLevelInfo, this.moduleName, 'JS:', sMessage); },

  // ------------------------------------------------
  // Babelfish
  
  Translate: function(sContext, sText)
  {
    if (sText == null) {
      sText = sContext;
      sContext = '';
    }
    
    var sTranslated = this.Message('Translation_Get')
      .setString('sModule', this.moduleName)
      .setString('sContext', sContext)
      .setString('sText', sText)
      .send()
      .getString('sTranslated');
      
      if (sTranslated == '') {
        return sText;
      } else {
        return sTranslated;
      }
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
  
  OnMouseDown: function(ev)
  {
    this.Message('WebView_MouseCapture').send();
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

  OnMouseMove: function(ev)
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
          this.Message('WebView_MoveBy').setInt('nX', dx).setInt('nY', dy).send();
        }
        if (this.bIsSize) {
          if (dx < 0 || dy < 0) {
            var pos = this.Message('WebView_GetPosition').send();
            var nW = pos.getInt('nWidth');
            var nH = pos.getInt('nHeight');
            var nNewW = nW + dx;
            var nNewH = nH + dy;
            if (nNewW < this.nMinWidth) { dx = this.nMinWidth - nW; }
            if (nNewH < this.nMinHeight) { dy = this.nMinHeight - nH; }
          }

          this.Message('WebView_SizeBy')
            .setInt('nX', dx)
            .setInt('nY', dy)
            .setInt('nDirection', this.nSizeDirection)
            .send();

          this.nStartX += dx;
          this.nStartY += dy;
        }
      }
    }
  },

  OnMouseUp: function()
  {
    this.bMouseCaptured = false;
    this.bMouseActive = false;
    this.bIsMove = false;
    this.bIsSize = false;
    this.Message('WebView_MouseRelease').send();
  },

  InitMoveSize: function()
  {
    document.onmousedown = this.OnMouseDown.bind(this); 
    document.onmousemove = this.OnMouseMove.bind(this); 
    document.onmouseup = this.OnMouseUp.bind(this);
  },

  // ------------------------------------------------
  
  Init: function()
  {
  },

  _:0
}
