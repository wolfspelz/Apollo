using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SHDocVw;
using mshtml;
using System.IO;
using Microsoft.Win32;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Net.Sockets;

/*
C:\Windows\Microsoft.NET\Framework\v4.0.30319\regasm /codebase C:\Heiner\Apollo\src\navigation\Browser\InternetExplorer\AvatarNavigator\AvatarNavigator\bin\Debug\AvatarNavigator.dll
C:\Windows\Microsoft.NET\Framework\v4.0.30319\regasm /unregister C:\Heiner\Apollo\src\navigation\Browser\InternetExplorer\AvatarNavigator\AvatarNavigator\bin\Debug\AvatarNavigator.dll
*/

namespace AvatarNavigator
{
  [
      ComVisible(true),
      Guid("000AE629-7C1E-4d0d-9B69-B23D5BE5A6C0"),
      ClassInterface(ClassInterfaceType.None)
  ]
  public class BHO : IObjectWithSite
  {
    WebBrowser _browser = null;
    //HTMLDocument _document = null;
    string _sUrl = "";
    static int _nCnt = 0;
    static int _nInstance = -1;
    TcpClient _socket = null;

    #region Main

    public BHO()
    {
      _nInstance = _nCnt;
      Log("Ctor " + _nCnt++);
      //Log("Ctor");
    }

    int _nLogCnt = 0;
    public void Log(string s)
    {
      Debug.WriteLine("### " + _nInstance + " " + _nLogCnt++ + " " + s);
    }

    #endregion

    #region Callbacks

    public void _browser_OnBeforeNavigate2(object pDisp, ref object URL, ref object Flags, ref object TargetFrameName, ref object PostData, ref object Headers, ref bool Cancel)
    {
      if ((WebBrowser) pDisp == _browser) {
        string sUrl = URL.ToString();
        //Log("OnBeforeNavigate2 " + URL.ToString());
        HandleUrl(URL.ToString());
      } else {
        //Log("OnBeforeNavigate2 (ignored) " + URL.ToString());
      }
    }

    public void _browser_NavigateComplete2(object pDisp, ref object URL)
    {
      if ((WebBrowser) pDisp == _browser) {
        //Log("NavigateComplete2 " + URL.ToString());
        HandleUrl(URL.ToString());
      } else {
        //Log("NavigateComplete2 (ignored) " + URL.ToString());
      }
    }

    public void _browser_OnQuit()
    {
      Log("OnQuit");
      CloseConnection();
    }

    #endregion

    void HandleUrl(string sUrl)
    {
      if (sUrl != _sUrl) {
        _sUrl = sUrl;
        UrlChanged(sUrl);
      }
    }

    void UrlChanged(string sUrl)
    {
      Log("UrlChanged " + sUrl);

      SendData(sUrl);
    }

    void SendData(string sData)
    {
      var ns = GetConnection();
      if (ns != null) {
        byte[] aData = System.Text.Encoding.ASCII.GetBytes(sData);
        ns.Write(aData, 0, aData.Length);
      }
    }

    NetworkStream GetConnection()
    {
      try {
        if (_socket == null) {
          _socket = new TcpClient();
          _socket.Connect("osiris", 12345);
        }
        if (_socket != null) {
          return _socket.GetStream();
        }
      } catch (Exception ex) {
        Log("GetSocket Exception " + ex.Message);
      }
      return null;
    }

    void CloseConnection()
    {
      if (_socket != null) {
        using (_socket) {
          NetworkStream ns = GetConnection();
          ns.Close();
        }
        _socket = null;
      }
    }

    #region IObjectWithSite Members

    public int SetSite(object site)
    {
      if (site != null) {
        _browser = (WebBrowser) site;
        _browser.BeforeNavigate2 += new DWebBrowserEvents2_BeforeNavigate2EventHandler(_browser_OnBeforeNavigate2);
        _browser.NavigateComplete2 += new DWebBrowserEvents2_NavigateComplete2EventHandler(_browser_NavigateComplete2);
        _browser.OnQuit += new DWebBrowserEvents2_OnQuitEventHandler(_browser_OnQuit);
      } else {
        _browser.BeforeNavigate2 -= new DWebBrowserEvents2_BeforeNavigate2EventHandler(_browser_OnBeforeNavigate2);
        _browser.NavigateComplete2 -= new DWebBrowserEvents2_NavigateComplete2EventHandler(_browser_NavigateComplete2);
        _browser.OnQuit -= new DWebBrowserEvents2_OnQuitEventHandler(_browser_OnQuit);
        _browser = null;
      }

      return 0;
    }

    public int GetSite(ref Guid guid, out IntPtr ppvSite)
    {
      IntPtr punk = Marshal.GetIUnknownForObject(_browser);
      int hr = Marshal.QueryInterface(punk, ref guid, out ppvSite);
      Marshal.Release(punk);

      return hr;
    }

    #endregion

    #region Register BHO
    public static string BHOKEYNAME = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects";

    [ComRegisterFunction]
    public static void RegisterBHO(Type type)
    {
      RegistryKey registryKey = Registry.LocalMachine.OpenSubKey(BHOKEYNAME, true);

      if (registryKey == null) {
        registryKey = Registry.LocalMachine.CreateSubKey(BHOKEYNAME);
      }

      string guid = type.GUID.ToString("B");
      RegistryKey ourKey = registryKey.OpenSubKey(guid);

      if (ourKey == null) {
        ourKey = registryKey.CreateSubKey(guid);
      }

      ourKey.SetValue("NoExplorer", 1);

      registryKey.Close();
      ourKey.Close();
    }

    [ComUnregisterFunction]
    public static void UnregisterBHO(Type type)
    {
      RegistryKey registryKey = Registry.LocalMachine.OpenSubKey(BHOKEYNAME, true);
      string guid = type.GUID.ToString("B");

      if (registryKey != null) {
        registryKey.DeleteSubKey(guid, false);
      }
    }

    #endregion
  }
}
