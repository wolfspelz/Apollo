
Function.prototype.bind = function(fn) { var m = this; return function() { return m.apply(fn, arguments); } }

function anLoad(e)
{
	var dummy = AvatarNavigator.getInstance();
}

function anPageHide(e)
{
  // There are many pagehide events, but only the ones from a XULDocument seem to be real close window events
  if (e.originalTarget instanceof XULDocument) {    
    //var s = ''; for ( var i in e) { s += i + '=' + e[i] + ' '; } anLogDebug(s);

    // Sometimes a fake close window comes thru the above filter
    // In FF 3.5.6 I can provoke that by having 1 tab, adding tab 2, navigate tab 2 to about:config, then navigate tab 2 to www.heise.de 
    //
    // Properties of fake close event:
    //   originalTarget=[object XPCNativeWrapper [object XULDocument]] preventCapture=function preventCapture() {\n    [native code]\n} target=[object XPCNativeWrapper [object XULDocument]] cancelable=true currentTarget=[object ChromeWindow] timeStamp=0 bubbles=true type=pagehide eventPhase=3 preventDefault=function preventDefault() {\n    [native code]\n} initEvent=function initEvent() {\n    [native code]\n} stopPropagation=function stopPropagation() {\n    [native code]\n} persisted=false initPageTransitionEvent=function initPageTransitionEvent() {\n    [native code]\n} CAPTURING_PHASE=1 AT_TARGET=2 BUBBLING_PHASE=3 explicitOriginalTarget=[object XPCNativeWrapper [object XULDocument]] preventBubble=function preventBubble() {\n    [native code]\n} isTrusted=true MOUSEDOWN=1 MOUSEUP=2 MOUSEOVER=4 MOUSEOUT=8 MOUSEMOVE=16 MOUSEDRAG=32 CLICK=64 DBLCLICK=128 KEYDOWN=256 KEYUP=512 KEYPRESS=1024 DRAGDROP=2048 FOCUS=4096 BLUR=8192 SELECT=16384 CHANGE=32768 RESET=65536 SUBMIT=131072 SCROLL=262144 LOAD=524288 UNLOAD=1048576 XFER_DONE=2097152 ABORT=4194304 ERROR=8388608 LOCATE=16777216 MOVE=33554432 RESIZE=67108864 FORWARD=134217728 HELP=268435456 BACK=536870912 TEXT=1073741824 ALT_MASK=1 CONTROL_MASK=2 SHIFT_MASK=4 META_MASK=8 
    // Properties of proper close event:
    //   originalTarget=[object XULDocument] preventCapture=function preventCapture() {\n    [native code]\n} target=[object XULDocument] cancelable=true currentTarget=[object ChromeWindow] timeStamp=0 bubbles=true type=pagehide eventPhase=2 preventDefault=function preventDefault() {\n    [native code]\n} initEvent=function initEvent() {\n    [native code]\n} stopPropagation=function stopPropagation() {\n    [native code]\n} persisted=false initPageTransitionEvent=function initPageTransitionEvent() {\n    [native code]\n} CAPTURING_PHASE=1 AT_TARGET=2 BUBBLING_PHASE=3 explicitOriginalTarget=[object XULDocument] preventBubble=function preventBubble() {\n    [native code]\n} isTrusted=true MOUSEDOWN=1 MOUSEUP=2 MOUSEOVER=4 MOUSEOUT=8 MOUSEMOVE=16 MOUSEDRAG=32 CLICK=64 DBLCLICK=128 KEYDOWN=256 KEYUP=512 KEYPRESS=1024 DRAGDROP=2048 FOCUS=4096 BLUR=8192 SELECT=16384 CHANGE=32768 RESET=65536 SUBMIT=131072 SCROLL=262144 LOAD=524288 UNLOAD=1048576 XFER_DONE=2097152 ABORT=4194304 ERROR=8388608 LOCATE=16777216 MOVE=33554432 RESIZE=67108864 FORWARD=134217728 HELP=268435456 BACK=536870912 TEXT=1073741824 ALT_MASK=1 CONTROL_MASK=2 SHIFT_MASK=4 META_MASK=8 

    // I know this is a bad hack. But this:
    //   if (!e.originalTarget instanceof XPCNativeWrapper)
    // ...does not work

    var bShutdown = true;    
    var sType = String(e.originalTarget);
    if (sType.search(/XPCNativeWrapper/g) >= 0) {
      bShutdown = false;
      anLogDebug('anPageHide Looks like pagehide of a XULDocument but is no native window close');
    }

    if (bShutdown) {
      AvatarNavigator.getInstance().shutdown();
      AvatarNavigator.deleteInstance();
    }
  }
}

function anToggle(e)
{
	AvatarNavigator.getInstance().toggle();
}

