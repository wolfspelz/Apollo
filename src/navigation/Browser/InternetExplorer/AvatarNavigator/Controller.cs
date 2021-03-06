using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Win32;
using System.IO;

namespace OVW
{
  public class Controller
  {
    bool _bAutoStart = true;
    string _sId = "";
    Random _rnd = new Random();
    string _sUrl = "";
    Client _client = null;
    int _nReconnectInterval = 20;
    int _nMinReconnectInterval = 20;
    int _nMaxReconnectInterval = 10000;
    bool _bConnected = false;
    System.Threading.Timer _connectTimer = null;
    bool _bInShutdown = false;
    string _sContext = "";
    bool _bShow = false;
    bool _bHasContext = false;
    string _sNativeVersion = "";
    int _nNativeHWND = 0;

    internal void Log(string s)
    {
      if (GetRegistryValue(Globals.Registry.Log, false)) {
        string sLine = s.Replace("\r\n", "\n").Replace("\n", "\\n");

        //SpicIE.Host.TraceSink.TraceInformation(sLine);

        if (!_bInShutdown) {
          try {
            using (StreamWriter sw = File.AppendText(GetRegistryValue(Globals.Registry.LogFile, "C:\\temp\\AvatarNavigator.log"))) {
              sw.WriteLine(sLine);
              sw.Close();
            }
          } catch { }
        }
      }
    }

    #region Utility // --------------------------------------

    bool TrueString(string sIn)
    {
      string s = sIn.ToLower();
      if (s == "no" || s == "0" || s == "false") { return false; }
      return s == "yes" || s == "true" || s == "1" || s == "on" || s == "ok" || s == "sure" || s == "yessir";
    }

    internal int GetRandomNumber(int nMax)
    {
      int nRandom = 0;
      lock (this) {
        nRandom = _rnd.Next(nMax);
      }
      return nRandom;
    }

    internal string GetRegistryValue(string sName, string sDefault)
    {
      string sResult = sDefault;
      try {
        using (var key = Registry.CurrentUser.OpenSubKey(@"Software\" + Globals.Manufacturer + @"\" + Globals.ProductName + "")) {
          if (key != null) {
            sResult = (string) key.GetValue(sName, sDefault);
          }
        }
      } catch { }
      return sResult;
    }

    internal int GetRegistryValue(string sName, int nDefault)
    {
      int nResult = 0;
      if (!Int32.TryParse(GetRegistryValue(sName, ""), out nResult)) {
        nResult = nDefault;
      }
      return nResult;
    }

    internal bool GetRegistryValue(string sName, bool bDefault)
    {
      bool bResult = false;
      string sValue = GetRegistryValue(sName, "");
      if (!String.IsNullOrEmpty(sValue)) {
        bResult = TrueString(sValue);
      }
      return bResult;
    }

    #endregion

    #region Main Flow // --------------------------------------

    internal void Run()
    {
      if (String.IsNullOrEmpty(_sId)) {
        _sId = this.GetHashCode() + "-" + DateTime.Now.Ticks + "-" + GetRandomNumber(1000000000);
      }

      _nMinReconnectInterval = GetRegistryValue(Globals.Registry.MinReconnectInterval, 20);
      _nMaxReconnectInterval = GetRegistryValue(Globals.Registry.MaxReconnectInterval, 10000);

      _bAutoStart = !GetRegistryValue(Globals.Registry.Disabled, false);

      if (_bAutoStart) {
        _nReconnectInterval = _nMinReconnectInterval;
        StartConnectTimer();
      }
    }

    void StartConnectTimer()
    {
      lock (this) {
        if (_connectTimer == null) {
          _nReconnectInterval *= 2;
          if (_nReconnectInterval > _nMaxReconnectInterval) {
            _nReconnectInterval = _nMaxReconnectInterval;
          }
          _connectTimer = new System.Threading.Timer(OnConnectTimer, null, _nReconnectInterval, System.Threading.Timeout.Infinite);
        }
      }
    }

    void OnConnectTimer(object state)
    {
      lock (this) {
        _connectTimer = null;
        Connect();
      }
    }

    internal void OnShutdown()
    {
      _bInShutdown = true;
      if (_client != null) {
        _client.Disconnect();
      }
    }

    void Connect()
    {
      if (_client == null) {
        _bConnected = false;
        _bHasContext = false;

        _client = new Client(this);
        _client.Connect(OnConnected, OnDisconnected);
      }
    }

    void Disconnect()
    {
      _client.Disconnect();
      _client = null;
    }

    #endregion

    #region Networking events // -----------------------------------

    void OnGetHandle(Srpc.Message response)
    {
      _sContext = response.GetString("hResult");
      _bHasContext = true;

      SendContextOpen();

      // Before Show, so that the browser window is known and can manage the Show
      if (_nNativeHWND != 0) {
        SendContextNativeWindow(_sNativeVersion, _nNativeHWND);
      }

      if (_bShow) {
        SendContextShow();
      } else {
        SendContextHide();
      }

      //SendContextPosition(200, 800);
      //SendContextSize(600, 400);
    }

    void OnHello(Srpc.Message response)
    {
      var srpc = new Srpc.Message();
      srpc.Set(Srpc.Key.Method, "System_GetHandle");
      _client.Send(srpc, OnGetHandle);
    }

    void OnConnected()
    {
      _bConnected = true;
      _nReconnectInterval = _nMinReconnectInterval;

      lock (this) {
        if (_client != null) {
          var srpc = new Srpc.Message();
          srpc.Set(Srpc.Key.Method, "Navigation_NavigatorHello");
          _client.Send(srpc, OnHello);
        }
      }
    }

    void OnDisconnected()
    {
      _bConnected = false;
      _client = null;

      if (_bInShutdown) {
        Log("OnDisconnected: skipping reconnect in shutdown");
      } else {
        StartConnectTimer();
      }
    }

    void Send(Srpc.Message srpc)
    {
      lock (this) {
        if (_client != null) {
          _client.Send(srpc);
        }
      }
    }

    #endregion

    #region Tab events // ------------------------------------

    internal void OnTabCreate()
    {
      Log("OnTabCreate " + _sUrl);
    }

    internal void OnTabNativeWindow(string sVersion, int nHWND)
    {
      Log("OnTabNativeWindow " + sVersion + " HWND=" + nHWND);

      if (_nNativeHWND != nHWND) {
        _sNativeVersion = sVersion;
        _nNativeHWND = nHWND;

        if (_bConnected && _bHasContext) {
          SendContextNativeWindow(_sNativeVersion, _nNativeHWND);
        }
      }

    }

    internal void OnTabDestroy()
    {
      Log("OnTabDestroy " + _sUrl);
    }

    internal void OnTabShow()
    {
      Log("OnTabShow " + _sUrl);

      if (!_bShow) {
        _bShow = true;
        if (_bConnected && _bHasContext) {
          SendContextShow();
        }
      }
    }

    internal void OnTabHide()
    {
      Log("OnTabHide " + _sUrl);

      if (_bShow) {
        _bShow = false;
        if (_bConnected && _bHasContext) {
          SendContextHide();
        }
      }
    }

    internal void OnTabNavigate(string sUrl)
    {
      Log("OnTabNavigate " + sUrl);

      if (_sUrl != sUrl) {
        _sUrl = sUrl;
        if (_bConnected && _bHasContext) {
          SendContextNavigate(sUrl);
        }
      }
    }

    #endregion

    #region Send Apollo messages // ---------------------------------------

    void SendContextOpen()
    {
      var srpc = new Srpc.Message();
      srpc.Set(Srpc.Key.Method, "Navigation_ContextOpen");
      srpc.Set("hContext", _sContext);
      Send(srpc);
    }

    void SendContextShow()
    {
      var srpc = new Srpc.Message();
      srpc.Set(Srpc.Key.Method, "Navigation_ContextShow");
      srpc.Set("hContext", _sContext);
      Send(srpc);
    }

    void SendContextHide()
    {
      var srpc = new Srpc.Message();
      srpc.Set(Srpc.Key.Method, "Navigation_ContextHide");
      srpc.Set("hContext", _sContext);
      Send(srpc);
    }

    void SendContextNavigate(string sUrl)
    {
      var srpc = new Srpc.Message();
      srpc.Set(Srpc.Key.Method, "Navigation_ContextNavigate");
      srpc.Set("hContext", _sContext);
      srpc.Set("sUrl", sUrl);
      Send(srpc);
    }

    void SendContextPosition(int nLeft, int nBottom)
    {
      var srpc = new Srpc.Message();
      srpc.Set(Srpc.Key.Method, "Navigation_ContextPosition");
      srpc.Set("hContext", _sContext);
      srpc.Set("nLeft", nLeft);
      srpc.Set("nBottom", nBottom);
      Send(srpc);
    }

    void SendContextSize(int nWidth, int nHeight)
    {
      var srpc = new Srpc.Message();
      srpc.Set(Srpc.Key.Method, "Navigation_ContextSize");
      srpc.Set("hContext", _sContext);
      srpc.Set("nWidth", nWidth);
      srpc.Set("nHeight", nHeight);
      Send(srpc);
    }

    void SendContextNativeWindow(string sVersion, int nHWND)
    {
      var sig = new Srpc.Message();
      sig.Set("sType", "InternetExplorer");
      sig.Set("sVersion", sVersion);
      sig.Set("nWin32HWND", nHWND);

      var srpc = new Srpc.Message();
      srpc.Set(Srpc.Key.Method, "Navigation_ContextNativeWindow");
      srpc.Set("hContext", _sContext);
      srpc.Set("kvSignature", sig.ToString());
      Send(srpc);
    }

    #endregion
  }
}
