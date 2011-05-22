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

  GetViewHandle: function()
  {
    return apollo.viewHandle; 
  },
  
  // ------------------------------------------------
  // SRPC

  OnDispatchMessage: function(msg) {},
  
  OnReceiveMessage: function (sMsg)
  {
    Log.IO('&lt; ' + sMsg);

    var msg = new ApMessage().fromString(sMsg);
    var resp = null;

    try {
      if (typeof(api.OnDispatchMessage) == 'function') {
        resp = api.OnDispatchMessage(msg);
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
      //if (msg.getString('Method') != 'Log_Line') { api.Log.Debug(sType + ' ' + apollo.viewHandle); }
      msg.setString('hView', apollo.viewHandle);
    }
    return msg;
  },

  // ------------------------------------------------
  // Log
  
  Log: {

    channelName: 'Unknown',
  
    Send: function(nMask, sChannel, sContext, sMessage)
    {
      return api.Message('Log_Line')
        .setInt('nMask', nMask)
        .setString('sChannel', sChannel)
        .setString('sContext', sContext)
        .setString('sMessage', sMessage)
        .send();
    },

    LevelFatal  :   2,
    LevelError  :   4,
    LevelWarning:   8,
    LevelUser   :  16,
    LevelDebug  :  32,
    LevelInfo   :  64,
    LevelVerbose: 128,

    Error: function(sMessage) { api.Log.Send(api.Log.LevelError, api.Log.moduleName, 'JS:', String(sMessage)); },
    Warning: function(sMessage) { api.Log.Send(api.Log.LevelWarning, api.Log.moduleName, 'JS:', String(sMessage)); },
    User: function(sMessage) { api.Log.Send(api.Log.LevelUser, api.Log.moduleName, 'JS:', String(sMessage)); },
    Debug: function(sMessage) { api.Log.Send(api.Log.LevelDebug, api.Log.moduleName, 'JS:', String(sMessage)); },
    Info: function(sMessage) { api.Log.Send(api.Log.LevelInfo, api.Log.moduleName, 'JS:', String(sMessage)); },

    _:0
  },
  
  // ------------------------------------------------
  // Babelfish
  
  Translate: function(sContext, sText)
  {
    if (sText == null) {
      sText = sContext;
      sContext = '';
    }
    
    var sTranslated = api.Message('Translation_Get')
      .setString('sModule', api.moduleName)
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
    api.bIsMove = false; 
    api.bIsSize = false; 

    var aClasses = ev.target.className.split(' ');
    if (aClasses != null) {
      for (var i = 0; i < aClasses.length; i++) {
        switch (aClasses[i]) {
          case 'cApMove': api.bIsMove = true; break;
//          case 'cApSizeLeft': api.bIsSize = true; api.nSizeDirection = 1; break;
//          case 'cApSizeTop': api.bIsSize = true; api.nSizeDirection = 2; break;
          case 'cApSizeRight': api.bIsSize = true; api.nSizeDirection = 3; break;
          case 'cApSizeBottom': api.bIsSize = true; api.nSizeDirection = 4; break;
//          case 'cApSizeTopLeft': api.bIsSize = true; api.nSizeDirection = 5; break;
//          case 'cApSizeTopRight': api.bIsSize = true; api.nSizeDirection = 6; break;
//          case 'cApSizeBottomLeft': api.bIsSize = true; api.nSizeDirection = 7; break;
          case 'cApSizeBottomRight': api.bIsSize = true; api.nSizeDirection = 8; break;
        }
      }
    }
    
    if (api.bIsMove || api.bIsSize) {    
      //<div id="Cover" style=";left:8px;top:24px;right:8px;bottom:8px;background-color:#FFFFFF;opacity:0.01;z-index:99;"></div>
      var eDiv = document.createElement('DIV');
      eDiv.setAttribute('id', 'cApCover');
      eDiv.style.cursor = ev.target.style.cursor;
      eDiv.style.position = 'absolute';
      eDiv.style.left = '0';
      eDiv.style.top = '0';
      eDiv.style.right = '0';
      eDiv.style.bottom = '0';
      eDiv.style.backgroundColor = '#FFFFFF';
      eDiv.style.opacity = '0.05';
      eDiv.style.zIndex = '99';
      document.body.appendChild(eDiv);
      
      api.Message('WebView_MouseCapture').send();
      api.nStartX = ev.x;
      api.nStartY = ev.y;
      api.bMouseActive = false;
      api.bMouseCaptured = true;
    }

    return false;
  },

  OnMouseMove: function(ev)
  {
    if (api.bMouseCaptured) {
      var dx = ev.x - api.nStartX;
      var dy = ev.y - api.nStartY;
      if (!api.bMouseActive) {
        if (dx > 4 || dx < -4 || dy > 4 || dy < -4) {
          api.bMouseActive = true;
        }
      }
      if (api.bMouseActive) {
        if (api.bIsMove) {
          api.Message('WebView_MoveBy').setInt('nX', dx).setInt('nY', dy).send();
        }
        if (api.bIsSize) {
          if (dx < 0 || dy < 0) {
            var pos = api.Message('WebView_GetPosition').send();
            var nW = pos.getInt('nWidth');
            var nH = pos.getInt('nHeight');
            var nNewW = nW + dx;
            var nNewH = nH + dy;
            if (nNewW < api.nMinWidth) { dx = api.nMinWidth - nW; }
            if (nNewH < api.nMinHeight) { dy = api.nMinHeight - nH; }
          }

          api.Message('WebView_SizeBy')
            .setInt('nX', dx)
            .setInt('nY', dy)
            .setInt('nDirection', api.nSizeDirection)
            .send();

          api.nStartX += dx;
          api.nStartY += dy;
        }
      }
    }

    return false;
  },

  OnMouseUp: function()
  {
    if (document.getElementById('cApCover')) {
      document.body.removeChild(document.getElementById('cApCover'));
    }
    
    api.bMouseCaptured = false;
    api.bMouseActive = false;
    api.bIsMove = false;
    api.bIsSize = false;
    api.Message('WebView_MouseRelease').send();

    return false;
  },

  InitMoveSize: function()
  {
    document.onmousedown = api.OnMouseDown.bind(this); 
    document.onmousemove = api.OnMouseMove.bind(this); 
    document.onmouseup = api.OnMouseUp.bind(this);
  },

  // ------------------------------------------------
  
  Init: function()
  {
  },

  _:0
}
