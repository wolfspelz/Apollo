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
using System.Net;
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
    Socket _socket = null;
    bool _bDoConnect = true;
    bool _bIsConnected = false;
    StringBuilder _sbSendBuffer = null;
    StringBuilder _sbReceiveBuffer = null;

    #region Main

    public BHO()
    {
      _nInstance = _nCnt;
      Log("Ctor " + _nCnt++);
      //Log("Ctor");

      if (_bDoConnect) {
        Connect();
      }
    }

    int _nLogCnt = 0;
    public void Log(string s)
    {
      Debug.WriteLine("### " + _nInstance + " " + _nLogCnt++ + " " + s.Replace("\n", "\\n"));
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
      Send(sUrl + "\n");
    }

    void Send(string sData)
    {
      if (_bIsConnected) {
        WriteDataToConnection(sData);
      } else {
        if (_sbSendBuffer == null) {
          _sbSendBuffer = new StringBuilder();
        }
        _sbSendBuffer.Append(sData);
        if (_bDoConnect) {
          Connect();
        }
      }
    }

    void WriteDataToConnection(string sData)
    {
      if (!_bIsConnected) {
        Log("WriteDataToConnection: not connected");
      } else {
        byte[] aData = System.Text.Encoding.UTF8.GetBytes(sData);
        _socket.BeginSend(aData, 0, aData.Length, SocketFlags.None, null, _socket);
      }
    }

    void Connect()
    {
      try {
        _bDoConnect = false;
        var socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        var host = Dns.GetHostEntry("osiris");
        var iep = new IPEndPoint(host.AddressList[0], 12345);
        socket.BeginConnect(iep, new AsyncCallback(OnConnected), socket);
      } catch (Exception ex) {
        Log("Connect: " + ex.Message);
      }
    }

    void OnConnected(IAsyncResult iar)
    {
      _socket = (Socket) iar.AsyncState;
      try {
        _socket.EndConnect(iar);
        _bIsConnected = true;

        ReadDataFromConnection();

        if (_sbSendBuffer != null && _sbSendBuffer.Length > 0) {
          WriteDataToConnection(_sbSendBuffer.ToString());
        }
        _sbSendBuffer = null;

      } catch (Exception ex) {
        _socket = null;
        _bDoConnect = true;
        Log("OnConnected: " + ex.Message);
      }
    }

    class ReceiveState
    {
      static int SIZE = 1024;
      public int nSize = SIZE;
      public byte[] aBuffer = new byte[SIZE];
      public Socket socket = null;
    }

    void ReadDataFromConnection()
    {
      var state = new ReceiveState();
      state.socket = _socket;
      _socket.BeginReceive(state.aBuffer, 0, state.nSize, SocketFlags.None, new AsyncCallback(OnReceiveData), state);
    }

    void OnReceiveData(IAsyncResult iar)
    {
      var state = (ReceiveState) iar.AsyncState;
      int nRead = state.socket.EndReceive(iar);

      if (state.socket.Connected == false || nRead <= 0) {
        OnDisconnected();
      } else {

        if (nRead > 0) {
          if (_sbReceiveBuffer == null) {
            _sbReceiveBuffer = new StringBuilder();
          }
          _sbReceiveBuffer.Append(Encoding.UTF8.GetString(state.aBuffer, 0, nRead));
          Log("OnReceiveData " + _sbReceiveBuffer.ToString());
          _sbReceiveBuffer = null;
        }

        ReadDataFromConnection();
      }
    }

    void OnDisconnected()
    {
      CloseConnection();
    }

    void CloseConnection()
    {
      if (_socket != null) {
        _socket.Close();
        _socket = null;
      }
      _bDoConnect = true;
      _bIsConnected = false;
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
