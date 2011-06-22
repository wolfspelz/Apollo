using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.IO;

namespace SpicIE.AvatarNavigator
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

#if DEBUG
            Host.TraceSink.TraceInformation(string.Format("HtmlEvents.HtmlEvents - DEBUG {0} - {1}",
                System.Reflection.Assembly.GetExecutingAssembly().FullName, DateTime.Now.TimeOfDay.ToString()));
#else
            Host.TraceSink.TraceInformation(string.Format("HtmlEvents.HtmlEvents - RELEASE {0} - {1}",
                System.Reflection.Assembly.GetExecutingAssembly().FullName, DateTime.Now.TimeOfDay.ToString()));
#endif

            // this is sample for using an event
            // simply attach the event to your custom eventhandler function
            this.OnDocumentComplete += new SpicIE.Common.WebBrowserEvent_DocumentComplete(AvatarNavigator_OnDocumentComplete);
            this.OnBeforeNavigate += new SpicIE.Common.WebBrowserEvent_BeforeNavigate2(AvatarNavigator_OnBeforeNavigate);
            this.OnWindowStateChange += new Common.WebBrowserEvents_WindowStateChange(AvatarNavigator_OnWindowStateChange);
            this.OnQuit += new Common.WebBrowserEvent_OnQuit(AvatarNavigator_OnQuit);

            this.OnNavigateComplete += new Common.WebBrowserEvent_NavigateComplete2(AvatarNavigator_OnNavigateComplete);
        }

        #endregion

        #region Events // ------------------------------------------

        void AvatarNavigator_OnNavigateComplete(object pDisp, ref object URL)
        {
            //Log("OnNavigateComplete " + _sId + " " + URL);
        }

        void AvatarNavigator_OnQuit()
        {
            //Log("OnQuit " + _sId);
            OnTabDestroy();

            Controller.OnShutdown();
        }

        void AvatarNavigator_OnWindowStateChange(uint dwWindowStateFlags, uint dwValidFlagsMask)
        {
            if (_nState != dwWindowStateFlags)
            {
                //Log("OnWindowStateChange " + _sId + " " + _nState + " -> =" + dwWindowStateFlags);
                _nState = dwWindowStateFlags;

                if (_nState == 3)
                {
                    if (_bCreated)
                    {
                        OnTabShow();
                    }
                    else
                    {
                        _bDoShowAfterCreate = true;
                    }
                }
                else if (_nState == 2)
                {
                    if (_bCreated)
                    {
                        OnTabHide();
                    }
                }
            }
        }

        void AvatarNavigator_OnBeforeNavigate(object pDisp, ref object URL, ref object Flags, ref object TargetFrameName, ref object PostData, ref object Headers, ref bool Cancel)
        {
            //Log("OnBeforeNavigate " + _sId + " URL=" + URL);
        }

        private void AvatarNavigator_OnDocumentComplete(object pDisp, ref object url)
        {
            try
            {
                //Log("OnDocumentComplete " + _sId + " " + url);

                SHDocVw.IWebBrowser2 browser = null;
                try { browser = HostInstance.BrowserRef as SHDocVw.IWebBrowser2; }
                catch { }

                if (browser != null)
                {
                    // get the IHTMLDocument interface

                    mshtml.IHTMLDocument2 htmlDoc = null;
                    try { htmlDoc = browser.Document as mshtml.IHTMLDocument2; }
                    catch { }

                    if (htmlDoc != null)
                    {
                        // Check if already there
                        object sensor = htmlDoc.all.item("iAvatarNavigatorSensor");
                        if (sensor != null)
                        {
                            //Log("Inject aborted " + _sId + " " + url);
                        }
                        else
                        {
                            //Log("Inject " + _sId + " " + url);

                            // Creeate custom element for DOM event for ContentBrowser-Extension communication
                            mshtml.IHTMLWindow2 parentWindow = htmlDoc.parentWindow;
                            if (parentWindow != null)
                            {
                                parentWindow.execScript(""
                                    + "var element = document.createElement('AvatarNavigator');"
                                    + "element.setAttribute('id', 'iAvatarNavigatorSensor');"
                                    + "document.documentElement.appendChild(element);"
                                    , "javascript"
                                );
                            }

                            if (!_bCreated)
                            {
                                _bCreated = true;
                                OnTabCreate();
                                if (_bDoShowAfterCreate)
                                {
                                    _bDoShowAfterCreate = false;
                                    OnTabShow();
                                }
                            }

                            // Get URL from document, the OnDocumentComplete(url) can be any sub-document
                            string sUrl = "no-url";
                            try { sUrl = htmlDoc.location.toString(); }
                            catch { }
                            OnTabNavigate(sUrl);

                        } // only once
                    }
                }
            }
            catch (Exception ex)
            {
                Log("OnDocumentComplete " + url + " " + ex.Message);
            }
        }

        #endregion

        #region Tab Events // ------------------------------------------

        private void OnTabCreate()
        {
            Controller.OnTabCreate();
        }

        private void OnTabDestroy()
        {
            Controller.OnTabDestroy();
        }

        //{
        // "method":"change_tab",
        // "data":{
        //   "tab_id":93
        // },
        // "dest":"background"
        //}
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
            Host.TraceSink.TraceEvent(TraceEventType.Information, 0, "RunOnceRegisterCOMControls");

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