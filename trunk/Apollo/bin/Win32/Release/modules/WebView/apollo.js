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

  fromString: function (sMsg) {
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

  encodeCString: function (sValue) {
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

  setString: function (sKey, sValue) {
    sValue = this.encodeCString(sValue);
    this.aParams_[sKey] = sValue;
    return this;
  },

  setInt: function (sKey, nValue) {
    this.aParams_[sKey] = String(nValue);
    return this;
  },

  setKeyValueList: function (sKey, kvList) {
    var s = '';
    for (var i in kvList) {
      s += i + '=' + this.encodeCString(kvList[i]) + '\n';
    }
    this.setString(sKey, s);
    return this;
  },

  // ---------------------
  // Getter

  toString: function () {
    var sMsg = '';
    for (var i in this.aParams_) {
      sMsg += i + '=' + this.aParams_[i] + '\n';
    }
    return sMsg;
  },

  decodeCString: function (sValue) {
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
          default: ;
        }
        sResult += sChar;
        bEscape = false;
      }
    }
    return sResult;
  },

  getString: function (sKey) {
    var sValue = '';
    if (this.aParams_[sKey]) {
      sValue = this.aParams_[sKey];
      sValue = this.decodeCString(sValue);
    }
    return sValue;
  },

  getInt: function (sKey) {
    return parseInt(this.getString(sKey));
  },

  getBool: function (sKey) {
    var b = false;
    var sValue = this.getString(sKey);
    if (0
    || sValue == '1'
    || sValue == 'true'
    || sValue == 'yes'
    || sValue == 'on'
    ) {
      b = true;
    }
    return b;
  },

  getKeyValueList: function (sKey) {
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

  send: function () {
    if (typeof (apollo) != 'undefined') {
      var sMsg = this.toString();
      if (typeof (this.onLog) == 'function') { this.onLog('&gt; ' + sMsg); }

      var sRespose = apollo.sendMessage(sMsg);
      if (typeof (this.onLog) == 'function') { this.onLog('&nbsp;&nbsp;&lt; ' + sRespose); }

      return new ApMessage().fromString(sRespose);
    }
  },

  // ---------------------
  // Test

  unitTest: function () {
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

  _avoidcommahazzle: 0
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

function ApEval(sText) { return eval(sText); }

var gApi = null;
function ApolloApi(sModule)
{
  this.moduleName = sModule;
  gApi = this;
}

ApolloApi.prototype =
{
  moduleName: 'Unknown',

  GetViewHandle: function () {
    return apollo.viewHandle;
  },

  // ------------------------------------------------
  // SRPC

  OnDispatchMessage: function (msg) { },

  OnReceiveMessage: function (sMsg) {
    Log.IO('&lt; ' + sMsg);

    var msg = new ApMessage().fromString(sMsg);
    var resp = null;

    try {
      if (typeof (this.OnDispatchMessage) == 'function') {
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

  Message: function (sMethod) {
    var msg = new ApMessage(sMethod);
    //if (msg.getString('Method') != 'Log_Line') { this.Log.Debug('apollo.js: ' + sType + ' ' + apollo.viewHandle); }
    msg.setString('hView', this.GetViewHandle());
    return msg;
  },

  ModuleCall: function (sMethod) {
    var msg = new ApMessage(sMethod);
    msg.setString('ApType', 'WebView_ModuleCall');
    return msg;
  },

  // ------------------------------------------------
  // Log

  Log: {
    Send: function (nMask, sChannel, sContext, sMessage) {
      return new ApMessage('Log_Line')
        .setInt('nMask', nMask)
        .setString('sChannel', sChannel)
        .setString('sContext', sContext)
        .setString('sMessage', sMessage)
        .send();
    },

    LevelFatal: 2,
    LevelError: 4,
    LevelWarning: 8,
    LevelUser: 16,
    LevelDebug: 32,
    LevelInfo: 64, 
    LevelVerbose: 128,

    Error: function (sMessage) { this.Send(this.LevelError, gApi.moduleName, 'JS:', String(sMessage)); },
    Warning: function (sMessage) { this.Send(this.LevelWarning, gApi.moduleName, 'JS:', String(sMessage)); },
    User: function (sMessage) { this.Send(this.LevelUser, gApi.moduleName, 'JS:', String(sMessage)); },
    Debug: function (sMessage) { this.Send(this.LevelDebug, gApi.moduleName, 'JS:', String(sMessage)); },
    Info: function (sMessage) { this.Send(this.LevelInfo, gApi.moduleName, 'JS:', String(sMessage)); },

    _avoidcommahazzle: 0
  },

  // ------------------------------------------------
  // Babelfish

  TranslateElement: function (oElement, sContext, sModule) {
    if (sModule == null) { sModule = this.moduleName; }
    if (sContext == null) { sContext = ''; }

    if (oElement.innerText && oElement.innerText != '') {
      //Log.Debug(oElement.innerText);
      oElement.innerText = this.Translate(oElement.innerText, sContext, sModule);
    }

    if (oElement.tagName && oElement.tagName == 'INPUT') {
      for (var i = 0; i < oElement.attributes.length; i++) {
        if (oElement.attributes[i].name == 'type') {
          if (oElement.attributes[i].value == 'button' || oElement.attributes[i].value == 'submit') {
            oElement.value = this.Translate(oElement.value, sContext, sModule);
            break;
          }
        }
      }
    }
  },

  // sContext may be empty
  // sModule is optional. sModule overrides the initial new ApolloApi(moduleName)
  Translate: function (sText, sContext, sModule) {
    if (sModule == null) { sModule = this.moduleName; }
    if (sContext == null) { sContext = ''; }

    var msg = new ApMessage('Translation_Get');
    msg.setString('sModule', sModule);
    msg.setString('sContext', sContext);
    msg.setString('sText', sText);
    var res = msg.send();
    var sTranslated = res.getString('sTranslated');

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

  OnMouseDown: function (ev) {
    //this.Log.Debug('apollo.js: ' + 'OnMouseDown ' + ev.target.className + ' ' + ev.x + ' ' + ev.y);

    this.bIsMove = false;
    this.bIsSize = false;

    var aClasses = ev.target.className.split(' ');
    if (aClasses != null) {
      for (var i = 0; i < aClasses.length; i++) {
        switch (aClasses[i]) {
          case 'cApMove': this.bIsMove = true; break;
          //          case 'cApSizeLeft': this.bIsSize = true; this.nSizeDirection = 1; break;       
          //          case 'cApSizeTop': this.bIsSize = true; this.nSizeDirection = 2; break;       
          case 'cApSizeRight': this.bIsSize = true; this.nSizeDirection = 3; break;
          case 'cApSizeBottom': this.bIsSize = true; this.nSizeDirection = 4; break;
          //          case 'cApSizeTopLeft': this.bIsSize = true; this.nSizeDirection = 5; break;       
          //          case 'cApSizeTopRight': this.bIsSize = true; this.nSizeDirection = 6; break;       
          //          case 'cApSizeBottomLeft': this.bIsSize = true; this.nSizeDirection = 7; break;       
          case 'cApSizeBottomRight': this.bIsSize = true; this.nSizeDirection = 8; break;
        }
      }
    }

    if (this.bIsMove || this.bIsSize) {
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
      eDiv.style.opacity = '0.01';
      eDiv.style.zIndex = '99';
      document.body.appendChild(eDiv);

      this.Message('WebView_MouseCapture').send();
      this.nStartX = ev.x;
      this.nStartY = ev.y;
      this.bMouseActive = false;
      this.bMouseCaptured = true;
    }

    return false;
  },

  OnMouseMove: function (ev) {
    if (!this.bMouseCaptured) {
      //this.Log.Debug('apollo.js: ' + 'OnMouseMove');
    } else {
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

    return false;
  },

  OnMouseUp: function () {
    //this.Log.Debug('apollo.js: ' + 'OnMouseUp');
    if (document.getElementById('cApCover')) {
      document.body.removeChild(document.getElementById('cApCover'));
    }

    this.bMouseCaptured = false;
    this.bMouseActive = false;
    this.bIsMove = false;
    this.bIsSize = false;
    this.Message('WebView_MouseRelease').send();

    return false;
  },

  InitMoveSize: function () {
    document.onmousedown = this.OnMouseDown.bind(this);
    document.onmousemove = this.OnMouseMove.bind(this);
    document.onmouseup = this.OnMouseUp.bind(this);
  },

  // ------------------------------------------------

  Init: function () {
  },

  _avoidcommahazzle: 0
}
