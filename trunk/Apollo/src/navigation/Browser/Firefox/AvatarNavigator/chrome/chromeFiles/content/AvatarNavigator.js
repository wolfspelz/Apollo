

var AvatarNavigator = function()
{
  this.prefManager = Components.classes["@mozilla.org/preferences-service;1"].getService(Components.interfaces.nsIPrefBranch);
  this.bConnected = false;
  this.sTest = 'Test';
  this.nTabCnt = 0;
  this.tabTimer = null;
  this.reconnectTimer = null;
  this.socket = null;
  this.nReconnectDelay = AvatarNavigator.MIN_RECONNECT_DELAY;
  this.protocol = null;
  this.selectedTab = null;
  
  this.tabListener =
  {
    QueryInterface: function(aIID)
    {
      if (aIID.equals(Components.interfaces.nsIWebProgressListener) ||
        aIID.equals(Components.interfaces.nsISupportsWeakReference) ||
        aIID.equals(Components.interfaces.nsISupports))
      { 
        return this;
      }

      throw Components.results.NS_NOINTERFACE;
    },

    parseFlags:function(aStateFlags)
    {
      var tag = "";
      var nsIWebProgressListener = Components.interfaces.nsIWebProgressListener;

      if (aStateFlags & nsIWebProgressListener.STATE_START) tag += "START";
      if (aStateFlags & nsIWebProgressListener.STATE_REDIRECTING) tag += "REDIR";
      if (aStateFlags & nsIWebProgressListener.STATE_TRANSFERRING) tag += "TRANS";
      if (aStateFlags & nsIWebProgressListener.STATE_STOP) tag += "STOP ";

      if (aStateFlags & nsIWebProgressListener.STATE_IS_REQUEST) tag += " | IS_REQ";
      else tag += "         ";
      if (aStateFlags & nsIWebProgressListener.STATE_IS_DOCUMENT) tag += " | IS_DOC";
      else tag += "         ";
      if (aStateFlags & nsIWebProgressListener.STATE_IS_NETWORK) tag += " | IS_NET";
      else tag += "         ";
      if (aStateFlags & nsIWebProgressListener.STATE_IS_WINDOW) tag += " | IS_WIN";

      return tag;
    },

    onStateChange: function(aWebProgress, aRequest, aFlag, aStatus)
    {
      //anLogDebug('onStateChange ' + this.parseFlags(aFlag));

      // If you use tabListener for more than one tab/window, use
      // aWebProgress.DOMWindow to obtain the tab/window which triggers the state change
      if (aFlag & Components.interfaces.nsIWebProgressListener.STATE_START) {
        // This fires when the load event is initiated
      }
      if (aFlag & Components.interfaces.nsIWebProgressListener.STATE_STOP) {
        // This fires when the load finishes
      }
    },

    onLocationChange: function(aWebProgress, aRequest, aURI)
    {
      if (aWebProgress.DOMWindow == aWebProgress.DOMWindow.top) {
        var browserIndex = gBrowser.getBrowserIndexForDocument(aWebProgress.DOMWindow.document);
        if (browserIndex != -1) {
          var tab = gBrowser.tabContainer.childNodes[browserIndex];
          AvatarNavigator.getInstance().processTabUrl(tab, aURI);
        }
      }
    },

    // For definitions of the remaining functions see related documentation
    onProgressChange: function(aWebProgress, aRequest, curSelf, maxSelf, curTot, maxTot)
    {
      //anLogDebug('onProgressChange ');
    },

    onStatusChange: function(aWebProgress, aRequest, aStatus, aMessage)
    {
      //anLogDebug('onStatusChange ');
    },

    onSecurityChange: function(aWebProgress, aRequest, aState)
    {
      //anLogDebug('onSecurityChange ');
    }
  } // this.tabListener

} // AvatarNavigator.ctor

AvatarNavigator.instance = null;
AvatarNavigator.DEFAULT_HOSTNAME = 'localhost';
AvatarNavigator.DEFAULT_PORT = 23763;
AvatarNavigator.MIN_RECONNECT_DELAY = 1000;
AvatarNavigator.MAX_RECONNECT_DELAY = 10000;
AvatarNavigator.TYPE_FIREFOX = 'Firefox';

AvatarNavigator.setTabContextRequested = function(tab, flag)
{
  if (flag) {
    tab.anContextRequested = flag;
  } else {
    delete tab.anContextRequested;
  }
}

AvatarNavigator.getTabContextRequested = function(tab)
{
  if (tab.anContextRequested != undefined) {
    return tab.anContextRequested;
  }
  return false;
}

AvatarNavigator.setTabHasNavigate = function(tab, flag)
{
  if (flag) {
    tab.anHasNavigate = flag;
  } else {
    delete tab.anHasNavigate;
  }
}

AvatarNavigator.getTabHasNavigate = function(tab)
{
  if (tab.anHasNavigate != undefined) {
    return tab.anHasNavigate;
  }
  return false;
}

AvatarNavigator.setTabContext = function(tab, hContext)
{
  if (hContext != '') {
    tab.anContext = hContext;
  } else {
    delete tab.anContext;
  }
}

AvatarNavigator.getTabContext = function(tab)
{
  if (tab.anContext && tab.anContext != '') {
    return tab.anContext;
  }
  return '';
}

AvatarNavigator.tabHasContext = function(tab)
{
  return AvatarNavigator.getTabContext(tab) != '';
}

AvatarNavigator.prototype.getHostname = function() { try { return this.prefManager.getCharPref('extensions.avatarnav.hostname'); } catch (ex) { return AvatarNavigator.DEFAULT_HOSTNAME; } }
AvatarNavigator.prototype.getPort = function() { try { return this.prefManager.getIntPref('extensions.avatarnav.port'); } catch (ex) { return AvatarNavigator.DEFAULT_PORT; } }
AvatarNavigator.prototype.getActive = function() { try { return this.prefManager.getBoolPref('extensions.avatarnav.active'); } catch (ex) { return true; } }

// -------------------------------

AvatarNavigator.prototype.createContextForTab = function(tab)
{
  var self = this;
  var fCallback = function(response)
  {
    if (response.getInt('Status') == 1) {
      var hContext = response.getString('hResult');
      if (hContext != '')  {

        AvatarNavigator.setTabContext(tab, hContext);
        gBrowser.getBrowserForTab(tab).addProgressListener(self.tabListener, Components.interfaces.nsIWebProgress.NOTIFY_STATE_DOCUMENT);
        
        self.processTabUrl(tab, gBrowser.getBrowserForTab(tab).currentURI);
      }
    }
  }

  anLogTrace('AvatarNavigator.createContextForTab');

  if (!AvatarNavigator.getTabContextRequested(tab)) {
    if (this.protocol) {
      AvatarNavigator.setTabContextRequested(tab, true);
      var msg = new SrpcMessage();
      msg.setString('Method', 'System_GetHandle');
      this.protocol.sendRequest(msg, fCallback);
    } else {
      anLogVerbose('AvatarNavigator.createContextForTab ignored(not-connected)');
    }
  }
}

AvatarNavigator.prototype.destroyContextForTab = function(tab)
{
  if (AvatarNavigator.tabHasContext(tab)) {
    AvatarNavigator.setTabContext(tab, '');
    AvatarNavigator.setTabContextRequested(tab, false);
    AvatarNavigator.setTabHasNavigate(tab, false);
    gBrowser.getBrowserForTab(tab).removeProgressListener(this.tabListener);
  }
}

AvatarNavigator.prototype.identifyContextForTab = function(tab, hContext)
{
  if (this.protocol) {
    var msg = new SrpcMessage();
    msg.setString('Method', 'Navigation_ContextNativeWindow');
    msg.setString('hContext', hContext);
    
    var kvSignature = new SrpcMessage();
    kvSignature.setString('sType', AvatarNavigator.TYPE_FIREFOX);
    kvSignature.setString('sVersion', navigator.userAgent);
    kvSignature.setString('sTitle', document.title);
    kvSignature.setString('nLeft', window.screenX);
    kvSignature.setString('nTop', window.screenY);
    kvSignature.setString('nInnerWidth', window.innerWidth);
    kvSignature.setString('nInnerHeight', window.innerHeight);
    msg.setString('kvSignature', kvSignature.toString());
    
    this.protocol.sendRequest(msg);    
  } else {
    anLogVerbose('AvatarNavigator.showContext ignored(not-connected) ' + hContext);
  }
}

// -------------------------------

AvatarNavigator.prototype.openContext = function(hContext)
{
  anLogInfo('OpenContext ' + hContext);

  if (this.protocol) {
    var msg = new SrpcMessage();
    msg.setString('Method', 'Navigation_ContextOpen');
    msg.setString('hContext', hContext);
    this.protocol.sendRequest(msg);
  } else {
    anLogVerbose('AvatarNavigator.openContext ignored(not-connected) ' + hContext);
  }
}

AvatarNavigator.prototype.navigateContext = function(hContext, sUrl)
{
  if (this.protocol) {
    anLogInfo('NavigateContext ' + hContext + ' ' + sUrl);
    var msg = new SrpcMessage();
    msg.setString('Method', 'Navigation_ContextNavigate');
    msg.setString('hContext', hContext);
    msg.setString('sUrl', sUrl);
    this.protocol.sendRequest(msg);
  } else {
    anLogVerbose('AvatarNavigator.navigateContext ignored(not-connected) ' + hContext + ' ' + sUrl);
  }
}

AvatarNavigator.prototype.closeContext = function(hContext)
{
  anLogInfo('CloseContext ' + hContext);

  if (this.protocol) {
    var msg = new SrpcMessage();
    msg.setString('Method', 'Navigation_ContextClose');
    msg.setString('hContext', hContext);
    this.protocol.sendRequest(msg);
  } else {
    anLogVerbose('AvatarNavigator.closeContext ignored(not-connected) ' + hContext);
  }
}

AvatarNavigator.prototype.showContext = function(hContext)
{
  anLogInfo('ShowContext ' + hContext);

  if (this.protocol) {
    var msg = new SrpcMessage();
    msg.setString('Method', 'Navigation_ContextShow');
    msg.setString('hContext', hContext);
    this.protocol.sendRequest(msg);    
  } else {
    anLogVerbose('AvatarNavigator.showContext ignored(not-connected) ' + hContext);
  }
}

AvatarNavigator.prototype.hideContext = function(hContext)
{
  anLogInfo('HideContext ' + hContext);

  if (this.protocol) {
    var msg = new SrpcMessage();
    msg.setString('Method', 'Navigation_ContextHide');
    msg.setString('hContext', hContext);
    this.protocol.sendRequest(msg);
  } else {
    anLogVerbose('AvatarNavigator.hideContext ignored(not-connected) ' + hContext);
  }
}

// -------------------------------

AvatarNavigator.prototype.processTabUrl = function(tab, url)
{
  if (AvatarNavigator.tabHasContext(tab)) {
    var hContext = AvatarNavigator.getTabContext(tab);
    anLogVerbose('AvatarNavigator.processTabUrl ' + hContext + ' ' + (url ? url.spec : ''));
    if (url && (0
    || url.scheme == 'http'
    || url.scheme == 'https'
    || url.scheme == 'ftp'
    )) {
      
      this.navigateContext(hContext, url.spec);
      
      // On first navigate
      if (!AvatarNavigator.getTabHasNavigate(tab)) {
        this.identifyContextForTab(tab, hContext);
      }
      
      AvatarNavigator.setTabHasNavigate(tab, true);

      if (tab == gBrowser.mCurrentTab) {
        this.showContext(hContext);
        
        if (this.selectedTab == null) {
          this.selectedTab = tab;
        }
        
      } else {
        this.hideContext(hContext);
      }

    } else {
      if (AvatarNavigator.getTabHasNavigate(tab)) {
        AvatarNavigator.setTabHasNavigate(tab, false);
        this.closeContext(hContext);
      } else {
        //this.openContext(hContext);
        //this.identifyContextForTab(tab, hContext);
        anLogVerbose('AvatarNavigator.processTabUrl ignored(no-navigate) ' + hContext + ' ' + (url?url.spec:''));
      }
    }
  }
}

AvatarNavigator.prototype.onTabOpen = function(e)
{
  anLogDebug('AvatarNavigator.onTabOpen');
  var tab = e.target;
  this.createContextForTab(tab);
}

AvatarNavigator.prototype.onTabClose = function(e)
{
  var tab = e.target;
  anLogDebug('AvatarNavigator.onTabClose ' + AvatarNavigator.getTabContext(tab));

  this.processTabUrl(tab, null);
  gBrowser.getBrowserForTab(tab).removeProgressListener(this.tabListener);
}

AvatarNavigator.prototype.onTabSelect = function(e)
{
  var tab = gBrowser.mCurrentTab;
  if (!AvatarNavigator.tabHasContext(tab)) {
    this.createContextForTab(tab);
  }
  
  anLogDebug('AvatarNavigator.onTabSelect ' + AvatarNavigator.getTabContext(tab));
  
  var hContext = '';
  if (AvatarNavigator.tabHasContext(tab)) {
    var hContext = AvatarNavigator.getTabContext(tab);
  }

  if (tab != this.selectedTab) {
    if (this.selectedTab != null) {
      //if (AvatarNavigator.tabHasContext(this.selectedTab)) {
        this.hideContext(AvatarNavigator.getTabContext(this.selectedTab));
      //}
    }
    this.selectedTab = tab;
    if (AvatarNavigator.getTabHasNavigate(tab)) {
      
      //var baseWindow = tab.linkedBrowser.contentWindow.top
      //                    .QueryInterface(Components.interfaces.nsIInterfaceRequestor)
      //                    .getInterface(Components.interfaces.nsIWebNavigation)
      //                    .QueryInterface(Components.interfaces.nsIDocShellTreeItem)
      //                    .treeOwner
      //                    .QueryInterface(Components.interfaces.nsIInterfaceRequestor)
      //                    .getInterface(Components.interfaces.nsIXULWindow)
      //                    .docShell
      //                    .QueryInterface(Components.interfaces.nsIBaseWindow);      
      //anLogDebug('baseWindow=' + String(baseWindow));

      //var o = tab; var s = String(o); anLogDebug(s);
      //var o = tab.linkedBrowser; var s = String(o); anLogDebug(s);
      //var o = tab.linkedBrowser.contentWindow; var s = String(o); anLogDebug(s);
      //var o = tab.linkedBrowser.contentWindow.top; var s = String(o); anLogDebug(s);
      //var o = tab.linkedBrowser.contentWindow.top.document; var s = String(o); anLogDebug(s);
      //var o = tab.linkedBrowser.contentWindow.top.document.title; var s = o; anLogDebug(s);
      //var o = tab.linkedBrowser.contentWindow; var s = o + ': \n'; for ( var i in o) { s += i + '=' + o[i] + '\n\n'; } anLogDebug(s);
      this.showContext(hContext);
    }
  }
}

AvatarNavigator.prototype.onTabTimer = function()
{
  //anLogDebug('AvatarNavigator.onTabTimer');

  var allTabs = gBrowser.mTabs;
  if (allTabs.length > 0) {
    for (var i = 0; i < allTabs.length; i++) {
      var tab = allTabs[i];
      if (!AvatarNavigator.tabHasContext(tab)) {
        //anLogDebug('AvatarNavigator.onTabTimer !tabHasContext i=' + i);
        this.createContextForTab(tab);
      }
    }
  }
}

AvatarNavigator.prototype.startTabTimer = function()
{
  this.tabTimer = setInterval(this.onTabTimer.bind(this), 2000);
}

AvatarNavigator.prototype.stopTabTimer = function()
{
  if (this.tabTimer != null) {
    clearInterval(this.tabTimer);
    this.tabTimer = null;
  }
}

AvatarNavigator.prototype.resetAllTabs = function()
{
  anLogTrace('AvatarNavigator.resetAllTabs');

  var allTabs = gBrowser.mTabs;
  if (allTabs.length > 0) {
    for (var i = 0; i < allTabs.length; i++) {
      var tab = allTabs[i];
      this.destroyContextForTab(tab);
    }
  }
}

// -------------------------------

AvatarNavigator.prototype.tcpConnect = function(sHostname, nPort)
{
  anLogTrace('AvatarNavigator.tcpConnect ' + sHostname + ':' + nPort);

  if (this.socket) {
    this.tcpDisconnect();
  }

  try {
    this.socket = new Socket();
    this.socket.connect(sHostname, nPort, this);
  } catch (ex) {
    anLogError('AvatarNavigator.tcpConnect ' + ex);
  }
}

AvatarNavigator.prototype.tcpDisconnect = function()
{
  anLogTrace('AvatarNavigator.tcpDisconnect');
  if (this.socket) {
    this.socket.disconnect();
    this.socket = null;
  }
}

AvatarNavigator.prototype.onSrpcHelloResponse = function()
{
  anLogTrace('AvatarNavigator.onSrpcHelloResponse');

  this.onTabTimer();
  this.startTabTimer();
}

AvatarNavigator.prototype.onConnected = function()
{
  anLogInfo('Connected');

  this.bConnected = true;
  this.nReconnectDelay = AvatarNavigator.MIN_RECONNECT_DELAY; // If it was connected, then reconnect quickly

  this.protocol = new SrpcProtocol(this);
  this.resetAllTabs();

// Check if 2-way communication is established
  var msg = new SrpcMessage();
  msg.setString('Method', 'Navigation_NavigatorHello');
  this.protocol.sendRequest(msg, this.onSrpcHelloResponse.bind(this));
}

AvatarNavigator.prototype.onDisconnected = function()
{
  if (this.bConnected) {
    anLogInfo('Disconnected');
  } else {
    anLogTrace('AvatarNavigator.onDisconnected');
  }

  this.bConnected = false;
  this.socket = null;
  this.protocol = null;
  this.stopTabTimer();

  if (this.reconnectTimer == null) {
    this.startReconnectTimer();
  }
}

AvatarNavigator.prototype.onReceiveData = function(sData)
{
  anLogVerbose('AvatarNavigator.onReceiveData: ' + sData);
  
  if (this.protocol) {
    this.protocol.parseData(sData);
  }
}

AvatarNavigator.prototype.onReceiveMessage = function(msg)
{
  anLogTrace('AvatarNavigator.onReceiveMessage');
}

AvatarNavigator.prototype.onSendData = function(sData)
{
  anLogTrace('AvatarNavigator.onSendData');
  this.send(sData);
}

// -------------------------------

AvatarNavigator.prototype.send = function(sData)
{
  if (this.socket) {
    anLogVerbose('AvatarNavigator.send: ' + sData);
    this.socket.send(sData);
  } else {
    anLogWarning('AvatarNavigator.send ignored(no-socket)');
  }
}

// -------------------------------

AvatarNavigator.prototype.onReconnectTimer = function()
{
  this.reconnectTimer = null;
  
  if (this.getActive()) {
    this.tcpConnect(this.getHostname(), this.getPort());
  } else {
    this.nReconnectDelay = 1000; // Not active then try every second
    this.startReconnectTimer();
  }
}

AvatarNavigator.prototype.nextReconnectDelay = function(nCurrentDelay)
{
  var nNextDelay = nCurrentDelay * 2;
  if (nNextDelay <= AvatarNavigator.MIN_RECONNECT_DELAY) {
    nNextDelay = AvatarNavigator.MIN_RECONNECT_DELAY;
  }
  if (nNextDelay >= AvatarNavigator.MAX_RECONNECT_DELAY) {
    nNextDelay = AvatarNavigator.MAX_RECONNECT_DELAY;
  }
  return nNextDelay;
}

AvatarNavigator.prototype.startReconnectTimer = function()
{
  anLogTrace('AvatarNavigator.startReconnectTimer this.nReconnectDelay=' + this.nReconnectDelay);
  this.reconnectTimer = setTimeout(this.onReconnectTimer.bind(this), this.nReconnectDelay);  
  this.nReconnectDelay = this.nextReconnectDelay(this.nReconnectDelay);
}

// -------------------------------

AvatarNavigator.prototype.cmdToggle = function()
{
  anLogTrace('AvatarNavigator.cmdToggle');
}

AvatarNavigator.prototype.cmdDisconnect = function()
{
  anLogTrace('AvatarNavigator.cmdDisconnect');
  this.tcpDisconnect();
}

AvatarNavigator.prototype.cmdShowDebug = function()
{
  anLogTrace('AvatarNavigator.cmdShowDebug');

  if (this.selectedTab != null) {
    var hContext = AvatarNavigator.getTabContext(this.selectedTab);
    if (this.protocol) {
      var msg = new SrpcMessage();
      msg.setString('ApType', 'Navigator_CallDisplay');
      msg.setString('Method', 'ShowDebug');
      msg.setString('hContext', hContext);
      msg.setInt('bShow', 1);
      this.protocol.sendRequest(msg);
    }
  }
}

AvatarNavigator.prototype.cmdShowChat = function()
{
  anLogTrace('AvatarNavigator.cmdShowChat');

  if (this.selectedTab != null) {
    var hContext = AvatarNavigator.getTabContext(this.selectedTab);
    if (this.protocol) {
      var msg = new SrpcMessage();
      msg.setString('ApType', 'Navigator_CallDisplay');
      msg.setString('Method', 'ShowChat');
      msg.setString('hContext', hContext);
      msg.setInt('bShow', 1);
      this.protocol.sendRequest(msg);
    }
  }
}

// -------------------------------

AvatarNavigator.prototype.init = function()
{
  anLogTrace('AvatarNavigator.init');

  getBrowser().tabContainer.addEventListener('TabOpen', this.onTabOpen.bind(this), false);
  getBrowser().tabContainer.addEventListener('TabClose', this.onTabClose.bind(this), false);
  getBrowser().tabContainer.addEventListener('TabSelect', this.onTabSelect.bind(this), false);
    
  this.tcpConnect(this.getHostname(), this.getPort());
}

AvatarNavigator.getInstance = function()
{
  if (AvatarNavigator.instance == null) {
    AvatarNavigator.instance = new AvatarNavigator();
    AvatarNavigator.instance.init();
  }
  return AvatarNavigator.instance;
}

AvatarNavigator.deleteInstance = function()
{
  if (AvatarNavigator.instance != null) {
    delete AvatarNavigator.instance;
    AvatarNavigator.instance = null;
  }
}

AvatarNavigator.prototype.shutdown = function()
{
  anLogDebug('AvatarNavigator.shutdown');

  var allTabs = gBrowser.mTabs;
  if (allTabs.length > 0) {
    for (var i = 0; i < allTabs.length; i++) {
      var tab = allTabs[i];
      this.processTabUrl(tab, null);
    }
  }

  this.tcpDisconnect();

  getBrowser().tabContainer.removeEventListener('TabOpen', this.onTabOpen.bind(this), false);
  getBrowser().tabContainer.removeEventListener('TabClose', this.onTabClose.bind(this), false);
  getBrowser().tabContainer.removeEventListener('TabSelect', this.onTabSelect.bind(this), false);
}

