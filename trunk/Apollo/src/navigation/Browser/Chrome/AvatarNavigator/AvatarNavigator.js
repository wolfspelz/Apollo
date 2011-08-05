var DEFAULT_HOSTNAME = 'localhost';
var DEFAULT_PORT = 23763;
var MIN_RECONNECT_DELAY = 1000;
var MAX_RECONNECT_DELAY = 10000;
var TYPE_CHROME = 'Chrome';

if (!window.console) console = {};
console.log = console.log || function () {};
console.warn = console.warn || function () {};
console.error = console.error || function () {};
console.info = console.info || function () {};

var aWindows = new Object();
var aTabs = new Object();

function Window(windowId)
{
  this.id = windowId;
  this.selectedTabId = '';
}

function AddWindow(windowId)
{
  console.log('AddWindow' + windowId);
  aWindows[windowId] = new Window(windowId);
}

function DeleteWindow(windowId)
{
  if (aWindows[windowId] != null) {
    console.log('DeleteWindow' + windowId);
    delete aWindows[windowId];
  }
}

function GetWindow(windowId) { return aWindows[windowId]; }
function HasWindow(windowId) { return aWindows[windowId] != null; }

function SetWindowSelectedTab(windowId, tabId)
{
  var myWindow = GetWindow(windowId);
  if (myWindow != null) {
    var currentTab = myWindow.selectedTab;
    if (currentTab != tabId) {
      if (currentTab != '') {
        OnTabHide(currentTab);
      }
      OnTabShow(tabId);
      myWindow.selectedTab = tabId;
    }
  }
}

function Tab(tabId)
{
  this.id = tabId;
  this.url = '';
  this.context = '';
}

function AddTab(tabId)
{
  console.log('AddTab ' + tabId);
  aTabs[tabId] = new Tab(tabId);
}

function DeleteTab(tabId)
{
  if (aTabs[tabId] != null) {
    console.log('DeleteTab ' + tabId);
    delete aTabs[tabId];
  }
}

function GetTab(tabId) { return aTabs[tabId]; }
function HasTab(tabId) { return aTabs[tabId] != null; }

function SetTabUrl(tabId, sUrl)
{
  var myTab = GetTab(tabId);
  if (myTab != null) {
    var sOldUrl = myTab.url;
    if (sOldUrl != sUrl) {
      myTab.url = sUrl;
      OnTabUrlChanged(tabId);
    }
  }
}

function OnTabShow(tabId)
{
  var myTab = GetTab(tabId);
  if (myTab != null) {
    console.log('OnTabShow ' + tabId);
  }
}

function OnTabHide(tabId)
{
  var myTab = GetTab(tabId);
  if (myTab != null) {
    console.log('OnTabHide ' + tabId);
  }
}

function OnTabUrlChanged(tabId)
{
  var myTab = GetTab(tabId);
  if (myTab != null) {
    console.log('OnTabUrlChanged ' + tabId + ' ctxt=' + myTab.context + ' url=' + myTab.url)
  }
}

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
    }
    chrome.tabs.getSelected(win.id, OnWindowGetSelectedTab);
  }
}

function OnWindowCreated(win)
{
  var windowId = win.id;
  //console.log('OnWindowCreated ' + 'left=' + win.left + ' top=' + win.top + ' width=' + win.width + ' height=' + win.height + ' ');
  if (win.type == 'normal') {
    AddWindow(windowId);
  }
}

function OnWindowRemoved(windowId)
{
  //console.log('OnWindowRemoved ' + windowId);
  DeleteWindow(windowId);
}

function OnTabCreated(tab)
{
  //console.log('OnTabCreated');
  ProcessTab(tab);
}

function OnTabRemoved(tabId, removeInfo)
{
  //console.log('OnTabRemoved ' + tabId);
  DeleteTab(tabId);
}

function OnTabUpdated(tabId, changeInfo, tab)
{
  //console.log('OnTabUpdated');
  ProcessTab(tab);
}

function OnTabSelectionChanged(tabId, selectInfo)
{
  //console.log('OnTabSelectionChanged');
  SetWindowSelectedTab(selectInfo.windowId, tabId);
}

function OnTabAttached(tabId, attachInfo)
{
  console.log('OnTabAttached');
}

function OnTabDetached(tabId, detachInfo)
{
  console.log('OnTabDetached');
}

function ProcessTab(tab)
{
  var tabId = tab.id;

  if (HasTab(tabId)) {
    //console.log('ProcessTab HasTab ' + GetTab(tabId).id);
  } else {
    AddTab(tabId);
  }

  if (HasTab(tabId)) {
    RequestTabContext(tabId);
  }

  if (HasTab(tabId) && tab.url != null) {
    var sUrl = tab.url;
    if (sUrl.indexOf('http://') === 0 || sUrl.indexOf('https://') === 0) {
      SetTabUrl(tabId, sUrl);
    }
  }
}

function RequestTabContext(tabId)
{
  var myTab = GetTab(tabId);
  if (myTab != null) {
    
  }
}

function Start()
{
//  chrome.windows.getAll({ 'populate': true }, OnWindowGetAllResult);

//  chrome.windows.onCreated.addListener(OnWindowCreated);
//  chrome.windows.onRemoved.addListener(OnWindowRemoved);
//  chrome.tabs.onCreated.addListener(OnTabCreated);
//  chrome.tabs.onRemoved.addListener(OnTabRemoved);
//  chrome.tabs.onUpdated.addListener(OnTabUpdated);
//  chrome.tabs.onSelectionChanged.addListener(OnTabSelectionChanged);
//  chrome.tabs.onAttached.addListener(OnTabAttached);
//  chrome.tabs.onDetached.addListener(OnTabDetached);

  var myWebSocket = new WebSocket("ws://localhost:23766");
  myWebSocket.onopen = function (evt) { myWebSocket.send("Method=System_GetHandle"); };
  myWebSocket.onmessage = function (evt) { alert("" + evt.data); };
  myWebSocket.onclose = function (evt) { alert("Connection closed."); };
}

// http://datatracker.ietf.org/doc/draft-ietf-hybi-thewebsocketprotocol/?include_text=1

Start();
