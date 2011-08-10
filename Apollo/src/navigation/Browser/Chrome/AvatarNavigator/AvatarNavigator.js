var DEFAULT_HOSTNAME = 'localhost';
var DEFAULT_PORT = 23765;
var MIN_RECONNECT_DELAY = 100;
var MAX_RECONNECT_DELAY = 10000;
var TYPE_CHROME = 'Chrome';

var sServerHostname = DEFAULT_HOSTNAME;
var nServerPort = DEFAULT_PORT + 1;
var nReconnectDelay = MIN_RECONNECT_DELAY;
var webSocket = null;
var srpcProtocol = null;

var aWindows = null;
var aTabs = null;

var anLogLevel = anLogLevelDebug;

// --------------------------
// Window

function Window(windowId, window)
{
  this.id = windowId;
  this.selectedTabId = null;
  this.window = window;
}

function AddWindow(windowId, window)
{
  anLogVerbose('AddWindow' + windowId);
  aWindows[windowId] = new Window(windowId, window);
}

function DeleteWindow(windowId)
{
  if (aWindows[windowId] != null) {
    anLogVerbose('DeleteWindow' + windowId);
    delete aWindows[windowId];
  }
}

function GetWindow(windowId) { return aWindows[windowId]; }
function HasWindow(windowId) { return aWindows[windowId] != null; }

function SetWindowSelectedTab(windowId, tabId)
{
  var myWindow = GetWindow(windowId);
  if (myWindow != null) {
    var currentTabId = myWindow.selectedTabId;
    if (currentTabId != null) {
      OnHideTab(currentTabId);
    }
    OnShowTab(tabId);
    myWindow.selectedTabId = tabId;
  }
}

// --------------------------
// Tab

function Tab(tabId, windowId, tab)
{
  this.id = tabId;
  this.windowId = windowId;
  this.sUrl = '';
  this.sContext = '';
  this.bVisible = false;
  this.tab = tab;
}

function AddTab(tabId, windowId, tab)
{
  anLogVerbose('AddTab ' + tabId);
  aTabs[tabId] = new Tab(tabId, windowId, tab);
}

function DeleteTab(tabId)
{
  OnCloseTab(tabId);
  if (aTabs[tabId] != null) {
    anLogVerbose('DeleteTab ' + tabId);
    delete aTabs[tabId];
  }
}

function GetTab(tabId) { return aTabs[tabId]; }
function HasTab(tabId) { return aTabs[tabId] != null; }

// --------------------------
// hContext

function RequestTabContext(tabId)
{
  var fCallback = function (response)
  {
    var myTab = GetTab(tabId);
    if (myTab != null) {
      if (response.getInt('Status') == 1) {
        var hContext = response.getString('hResult');
        if (hContext != '') {
          anLogInfo('TabContext tab=' + tabId + ' ctxt=' + hContext);
          myTab.sContext = hContext;

          OpenContext(myTab.sContext);

          IdentifyContext(myTab.sContext, myTab.windowId);

          if (myTab.bVisible) {
            ShowContext(myTab.sContext);
          } else {
            HideContext(myTab.sContext);
          }

          NavigateContext(myTab.sContext, myTab.sUrl);
        }
      }
    }
  }

  var myTab = GetTab(tabId);
  if (myTab != null) {
    if (srpcProtocol != null) {
      srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'System_GetHandle'), fCallback);
    } else {
      anLogWarning('RequestTabContext ignored(not-connected)');
    }
  }
}

// --------------------------
// Core tab events

function OnOpenTab(tabId)
{
  anLogInfo('OnOpenTab ' + tabId);
  var myTab = GetTab(tabId);
  if (myTab != null) {
    OpenContext(myTab.sContext);
  }
}

function OnShowTab(tabId)
{
  var bVisible = true;
  anLogInfo('OnShowTab ' + tabId);
  var myTab = GetTab(tabId);
  if (myTab != null) {
    var bOldVisible = myTab.bVisible;
    if (bOldVisible != bVisible) {
      myTab.bVisible = bVisible;
      if (myTab.sContext != '') {
        ShowContext(myTab.sContext);
      }
    }
  }
}

function OnHideTab(tabId)
{
  var bVisible = false;
  anLogInfo('OnHideTab ' + tabId);
  var myTab = GetTab(tabId);
  if (myTab != null) {
    var bOldVisible = myTab.bVisible;
    if (bOldVisible != bVisible) {
      myTab.bVisible = bVisible;
      if (myTab.sContext != '') {
        HideContext(myTab.sContext);
      }
    }
  }
}

function OnNavigateTab(tabId, sUrl)
{
  anLogInfo('OnNavigateTab ' + tabId + ' ' + sUrl);
  var myTab = GetTab(tabId);
  if (myTab != null) {
    var sOldUrl = myTab.sUrl;
    if (sOldUrl != sUrl) {
      myTab.sUrl = sUrl;
      if (myTab.sContext != '') {
        NavigateContext(myTab.sContext, sUrl);
      }
    }
  }
}

function OnReparentTab(tabId, newWindowId)
{
  anLogInfo('OnReparentTab tab=' + tabId + ' to win=' + newWindowId);
  var myTab = GetTab(tabId);
  if (myTab != null) {
    if (myTab.windowId != newWindowId) {
      myTab.windowId = newWindowId;
      IdentifyContext(myTab.sContext, newWindowId);
    }
  }
}

function OnCloseTab(tabId)
{
  anLogInfo('OnCloseTab ' + tabId);
  var myTab = GetTab(tabId);
  if (myTab != null) {
    if (myTab.sContext != '') {
      CloseContext(myTab.sContext);
    }
  }
}

// --------------------------
// Context

function OpenContext(sContext)
{
  anLogVerbose('OpenContext ' + sContext);
  if (srpcProtocol != null) {
    srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'Navigation_ContextOpen').setString('hContext', sContext));
  } else {
    anLogWarning('OpenContext ignored(not-connected) ' + sContext);
  }
}

function IdentifyContext(sContext, windowId)
{
  anLogVerbose('IdentifyContext' + sContext);
  var myWindow = GetWindow(windowId);
  if (myWindow != null) {
    if (srpcProtocol != null) {

      var msg = new SrpcMessage();
      msg.setString('Method', 'Navigation_ContextNativeWindow');
      msg.setString('hContext', sContext);

      var selectedTab = null;
      var aTabs = myWindow.window.tabs;
      if (aTabs != null) {
        for (var i = 0; i < aTabs.length; ++i) {
          var tab = aTabs[i];
          if (tab.selected) {
            selectedTab = tab;
          }
        }
      }
      var sTitle = '';
      if (selectedTab != null) {
        sTitle = selectedTab.title;
      }

      var kvSignature = new SrpcMessage();
      kvSignature.setString('sType', TYPE_CHROME);
      kvSignature.setString('sVersion', navigator.userAgent);
      if (sTitle != '') {
        kvSignature.setString('sTitle', sTitle);
      }
      kvSignature.setString('nLeft', myWindow.window.left);
      kvSignature.setString('nTop', myWindow.window.top);
      kvSignature.setString('nWidth', myWindow.window.width);
      kvSignature.setString('nHeight', myWindow.window.height);
      msg.setString('kvSignature', kvSignature.toString());

      srpcProtocol.sendRequest(msg);

    } else {
      anLogWarning('IdentifyContext ignored(not-connected) ' + sContext);
    }
  }
}

function ShowContext(sContext)
{
  anLogVerbose('ShowContext ' + sContext);
  if (srpcProtocol != null) {
    srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'Navigation_ContextShow').setString('hContext', sContext));
  } else {
    anLogWarning('ShowContext ignored(not-connected) ' + sContext);
  }
}

function HideContext(sContext)
{
  anLogVerbose('HideContext ' + sContext);
  if (srpcProtocol != null) {
    srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'Navigation_ContextHide').setString('hContext', sContext));
  } else {
    anLogWarning('HideContext ignored(not-connected) ' + sContext);
  }
}

function NavigateContext(sContext, sUrl)
{
  if (sUrl.indexOf('http://') == 0 || sUrl.indexOf('https://') == 0) {
  } else {
    sUrl = '';
  }

  anLogVerbose('NavigateContext ' + sContext + ' ' + sUrl)
  if (srpcProtocol != null) {
    srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'Navigation_ContextNavigate').setString('hContext', sContext).setString('sUrl', sUrl));
  } else {
    anLogWarning('NavigateContext ignored(not-connected) ' + sContext);
  }
}

function CloseContext(sContext)
{
  anLogVerbose('CloseContext ' + sContext)
  if (srpcProtocol != null) {
    srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'Navigation_ContextClose').setString('hContext', sContext));
  } else {
    anLogWarning('CloseContext ignored(not-connected) ' + sContext);
  }
}

// --------------------------
// chrome.tab and crome.window hooks

function OnWindowGetSelectedTab(tab)
{
  SetWindowSelectedTab(tab.windowId, tab.id);
}

function OnWindowGetAllResult(windows)
{
  var nWindows = windows.length;
  for (var i = 0; i < nWindows; i++) {
    var win = windows[i];
    OnWindowCreated(win);
    var nTabs = win.tabs.length;
    for (var j = 0; j < nTabs; j++) {
      var tab = win.tabs[j];
      OnTabCreated(tab);
      OnNavigateTab(tab.id, tab.url);
      if (tab.selected) {
        OnShowTab(tab.id);
      } else {
        OnHideTab(tab.id);
      }
    }
    chrome.tabs.getSelected(win.id, OnWindowGetSelectedTab);
  }
}

function OnWindowCreated(win)
{
  var windowId = win.id;
  //anLogVerbose('OnWindowCreated ' + 'left=' + win.left + ' top=' + win.top + ' width=' + win.width + ' height=' + win.height + ' ');
  if (win.type == 'normal') {
    AddWindow(windowId, win);
  }
}

function OnWindowRemoved(windowId)
{
  //anLogVerbose('OnWindowRemoved ' + windowId);
  DeleteWindow(windowId);
}

function OnTabCreated(tab)
{
  //anLogVerbose('OnTabCreated');
  var tabId = tab.id;
  if (!HasTab(tabId)) {
    AddTab(tabId, tab.windowId, tab);
    RequestTabContext(tabId);
  }
}

function OnTabRemoved(tabId, removeInfo)
{
  //anLogVerbose('OnTabRemoved ' + tabId);
  DeleteTab(tabId);
}

function OnTabUpdated(tabId, changeInfo, tab)
{
  //anLogVerbose('OnTabUpdated');
  if (HasTab(tabId) && tab.url != null) {
    OnNavigateTab(tabId, tab.url);
  }
}

function OnTabSelectionChanged(tabId, selectInfo)
{
  //anLogVerbose('OnTabSelectionChanged');
  SetWindowSelectedTab(selectInfo.windowId, tabId);
}

function OnTabDetached(tabId, detachInfo)
{
  anLogVerbose('OnTabDetached');
  var myTab = GetTab(tabId);
  if (myTab != null) {
//     if (myTab.bVisible) {
//       myTab.bAttachVisible = true;
//       OnHideTab(tabId);
//     }
  }
}

function OnTabAttached(tabId, attachInfo)
{
  anLogVerbose('OnTabAttached');
  var myTab = GetTab(tabId);
  if (myTab != null) {
    OnReparentTab(tabId, attachInfo.newWindowId);
//     if (myTab.bAttachVisible) {
//       myTab.bAttachVisible = false;
//       OnShowTab(tabId);
//     }
  }
}

// --------------------------
// Start

function StartTabs()
{
  aWindows = new Object();
  aTabs = new Object();

  chrome.windows.getAll({ 'populate': true }, OnWindowGetAllResult);

  chrome.windows.onCreated.addListener(OnWindowCreated);
  chrome.windows.onRemoved.addListener(OnWindowRemoved);
  chrome.tabs.onCreated.addListener(OnTabCreated);
  chrome.tabs.onRemoved.addListener(OnTabRemoved);
  chrome.tabs.onUpdated.addListener(OnTabUpdated);
  chrome.tabs.onSelectionChanged.addListener(OnTabSelectionChanged);
  chrome.tabs.onAttached.addListener(OnTabAttached);
  chrome.tabs.onDetached.addListener(OnTabDetached);
}

function Start()
{
  StartReconnectTimer();
}

function StartReconnectTimer()
{
  nReconnectDelay *= 2;
  if (nReconnectDelay > MAX_RECONNECT_DELAY) {
    nReconnectDelay = MAX_RECONNECT_DELAY;
  }
  anLogInfo('StartReconnectTimer: scheduling connect in  ' + nReconnectDelay + ' msec');
  window.setTimeout('Reconnect();', nReconnectDelay);
}

// --------------------------
// Connection

function OnReceiveMessage(msg)
{
  var sMethod = msg.getString('Method');
  anLogInfo('OnReceiveMessage Method=' + sMethod);
}

function OnSendData(sData)
{
  anLogInfo('OUT: ' + sData);
  webSocket.send(sData);
}

function OnHelloResponse(msg)
{
  StartTabs();
}

function OnWebSocketOpened(evt)
{
  anLogInfo('OnWebSocketOpened');
  nReconnectDelay = MIN_RECONNECT_DELAY;
  
  srpcProtocol = new SrpcProtocol();
  srpcProtocol.onReceiveMessage = OnReceiveMessage;
  srpcProtocol.onSendData = OnSendData;

  if (srpcProtocol != null) {
    srpcProtocol.sendRequest(new SrpcMessage().setString('Method', 'Navigation_NavigatorHello'), OnHelloResponse);
  }
}

function OnWebSocketMesssge(evt)
{
  var sData  = evt.data;
  anLogInfo('IN: ' + sData);
  srpcProtocol.handleMessage(sData);
}

function OnWebSocketClosed(evt)
{
  anLogInfo('OnWebSocketClosed');
  webSocket = null;
  srpcProtocol = null;
  StartReconnectTimer();
}

function Reconnect()
{
  var sUrl = 'ws://' + sServerHostname + ':' + nServerPort;
  anLogInfo('Connecting to ' + sUrl);
  webSocket = new WebSocket(sUrl);
  webSocket.onopen = OnWebSocketOpened;
  webSocket.onmessage = OnWebSocketMesssge;
  webSocket.onclose = OnWebSocketClosed;
}

// --------------------------

Start();
