using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.IO;

namespace OVW
{
  /// <summary>
  /// This is the base class of a custom plugin. It must derive from SpicIE.Host and contain the 
  /// attributes defined below to mark this class as a COM object. The attributes properties are
  /// build from the constants below which have to unique in order to successfully register the 
  /// class as a COM object. 
  /// </summary>
  [ComVisible(true),
  ClassInterface(ClassInterfaceType.AutoDual),
  Guid(AvatarNavigator.PLUGIN_GUID),
  ProgId(AvatarNavigator.PLUGIN_PROGID)]
  public class AvatarNavigator : SpicIE.Host
  {
    /// <summary>
    /// These two constants define the CLSID and name for the plugin.
    /// This must be unique otherwise you won't be able to control, install or deinstall your plugin
    /// When developing choose a static unique GUID and leave it as it is
    /// </summary>
    public const string PLUGIN_GUID = "A319F1F2-61EB-44c1-B3B0-9005C7D73843";
    public const string PLUGIN_PROGID = "AvatarNavigator";

    public static AvatarNavigator HostInstance = null;

    Controller Controller;

    uint _nState = 0;
    bool _bCreated = false;
    bool _bDoShowAfterCreate = false;

    #region Plugin Base Constructor

    void Log(string s)
    {
      Controller.Log(s);
    }

    public AvatarNavigator()
      : base()
    {
      HostInstance = this;

      Controller = new Controller();
      Controller.Run();

      Log(string.Format("AvatarNavigator {0} - {1}", System.Reflection.Assembly.GetExecutingAssembly().FullName, DateTime.Now.TimeOfDay.ToString()));


      this.OnDocumentComplete += new SpicIE.Common.WebBrowserEvent_DocumentComplete(AvatarNavigator_OnDocumentComplete);
      this.OnBeforeNavigate += new SpicIE.Common.WebBrowserEvent_BeforeNavigate2(AvatarNavigator_OnBeforeNavigate);
      this.OnWindowStateChange += new SpicIE.Common.WebBrowserEvents_WindowStateChange(AvatarNavigator_OnWindowStateChange);
      this.OnQuit += new SpicIE.Common.WebBrowserEvent_OnQuit(AvatarNavigator_OnQuit);
      this.OnNavigateComplete += new SpicIE.Common.WebBrowserEvent_NavigateComplete2(AvatarNavigator_OnNavigateComplete);

    }

    #endregion

    #region Events // ------------------------------------------

    void AvatarNavigator_OnNavigateComplete(object pDisp, ref object URL)
    {
      //Log("OnNavigateComplete " + URL);
    }

    void AvatarNavigator_OnQuit()
    {
      //Log("OnQuit " + _sId);
      OnTabDestroy();

      Controller.OnShutdown();
    }

    void AvatarNavigator_OnWindowStateChange(uint dwWindowStateFlags, uint dwValidFlagsMask)
    {
      if (_nState != dwWindowStateFlags) {
        //Log("OnWindowStateChange " + _nState + " -> =" + dwWindowStateFlags);
        _nState = dwWindowStateFlags;

        if (_nState == 3) {
          if (_bCreated) {
            OnTabShow();
          } else {
            _bDoShowAfterCreate = true;
          }
        } else if (_nState == 2) {
          if (_bCreated) {
            OnTabHide();
          }
        }
      }
    }

    void AvatarNavigator_OnBeforeNavigate(object pDisp, ref object URL, ref object Flags, ref object TargetFrameName, ref object PostData, ref object Headers, ref bool Cancel)
    {
      //Log("OnBeforeNavigate URL=" + URL);
    }

    private void AvatarNavigator_OnDocumentComplete(object pDisp, ref object url)
    {
      try {
        //Log("OnDocumentComplete " + url);

        SHDocVw.IWebBrowser2 browser = null;
        try { browser = HostInstance.BrowserRef as SHDocVw.IWebBrowser2; } catch (Exception ex) { Log("HostInstance.BrowserRef as SHDocVw.IWebBrowser2 " + ex.Message); }

        if (browser != null) {
          Log("OnDocumentComplete: " + url + " hwnd=" + browser.HWND + " left=" + browser.Left + " top=" + browser.Top + " width=" + browser.Width + " height=" + browser.Height + " LocationURL=" + browser.LocationURL);

          mshtml.IHTMLDocument2 document = null;
          try { document = browser.Document as mshtml.IHTMLDocument2; } catch (Exception ex) { Log("browser.Document as mshtml.IHTMLDocument2 " + ex.Message); }

          if (document != null) {

            // Get URL from document, the OnDocumentComplete(url) can be any sub-document
            string sDocumentUrl = "no-url";
            try { sDocumentUrl = document.location.toString(); } catch { }

            Log("OnDocumentComplete: " + url + " document.location=" + sDocumentUrl + " browser.LocationURL=" + browser.LocationURL);

            // Check if already there
            object sensorAlreadyInserted = document.all.item("iAvatarNavigatorSensor");
            if (sensorAlreadyInserted != null) {
              //Log("OnDocumentComplete aborted " + url);
            } else {
              //var process = Process.GetCurrentProcess();
              //string sWnd = process.MainWindowHandle.ToString();
              //string sTitle = process.MainWindowTitle;
              //Log("Window: " + sWnd + " " + sTitle);

              mshtml.IHTMLWindow2 parentWindow = document.parentWindow;
              if (parentWindow != null) {

                // Inject a marker to check if already processed
                parentWindow.execScript(""
                    + "var element = document.createElement('AvatarNavigator');"
                    + "element.setAttribute('id', 'iAvatarNavigatorSensor');"
                    + "document.documentElement.appendChild(element);"
                    , "javascript"
                );

                if (!_bCreated) {
                  _bCreated = true;
                  OnTabCreate();

                  string sVersion = "";
                  try { sVersion = parentWindow.navigator.appVersion.ToString(); } catch (Exception ex) { Log("OnDocumentComplete Exception parentWindow.navigator.appVersion.ToString() " + ex.Message); }

                  int nHWND = 0;
                  try { nHWND = browser.HWND; } catch (Exception ex) { Log("OnDocumentComplete Exception in browser.HWND " + ex.Message); }

                  OnTabNativeWindow(sVersion, nHWND);

                  if (_bDoShowAfterCreate) {
                    _bDoShowAfterCreate = false;
                    OnTabShow();
                  }
                }
              }

              OnTabNavigate(sDocumentUrl);

            } // only once
          }
        }
      } catch (Exception ex) {
        Log("OnDocumentComplete " + url + " " + ex.Message);
      }
    }

    #endregion

    #region Tab Events // ------------------------------------------

    private void OnTabCreate()
    {
      Controller.OnTabCreate();
    }

    private void OnTabNativeWindow(string sVersion, int nHWND)
    {
      Controller.OnTabNativeWindow(sVersion, nHWND);
    }

    private void OnTabDestroy()
    {
      Controller.OnTabDestroy();
    }

    private void OnTabShow()
    {
      Controller.OnTabShow();
    }

    private void OnTabHide()
    {
      Controller.OnTabHide();
    }

    private void OnTabNavigate(string sUrl)
    {
      Controller.OnTabNavigate(sUrl);
    }

    #endregion

    #region Register your new browser UI elements here

    /// <summary>
    /// In this function you can instanciate your new custom created controls like toolbars, menus, etc
    /// the controls will be added to the controlscollection and returned to be available in the plugin
    /// </summary>
    /// <returns></returns>
    internal static List<SpicIE.Controls.IControlBase> RunOnceCOMRegistration()
    {
      SpicIE.Host.TraceSink.TraceEvent(TraceEventType.Information, 0, "RunOnceRegisterCOMControls");

      List<SpicIE.Controls.IControlBase> controls = new List<SpicIE.Controls.IControlBase>();

      return controls;
    }

    #endregion


    #region SpicIE - Required methods, do not change

    /// <summary>
    /// This function is necessary to register this plugin using the functions provided in the SpicIE.Host
    /// The function and its contents must not be changed
    /// </summary>
    /// <param name="typeToRegister"></param>
    [ComRegisterFunction]
    internal static void RegisterControls(Type typeToRegister)
    {
      SpicIE.Host.RegisterHost(typeToRegister, PLUGIN_GUID, PLUGIN_PROGID);
      SpicIE.Host.RegisterControls(RunOnceCOMRegistration());
    }

    /// <summary>
    /// This function is necessary to unregister this plugin using the functions provided in the SpicIE.Host
    /// The function and its contents must not be changed
    /// </summary>
    /// <param name="typeToRegister"></param>
    [ComUnregisterFunction]
    internal static void UnregisterControls(Type typeToRegister)
    {
      SpicIE.Host.UnregisterHost(typeToRegister, PLUGIN_GUID);
      SpicIE.Host.UnregisterControls(RunOnceCOMRegistration());
    }

    #endregion
  }
}