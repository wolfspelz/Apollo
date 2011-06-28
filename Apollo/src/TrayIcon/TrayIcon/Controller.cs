using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;
using Microsoft.VisualBasic.ApplicationServices;
using System.Diagnostics;

namespace TrayIcon
{
  public class Controller : WindowsFormsApplicationBase
  {
    Form1 _form = null;

    const bool DEFAULT_VISIBLE = false;
    const bool DEFAULT_AUTOCONNECT = true;
    const bool DEFAULT_AUTOSTART = true;
    const string DEFAULT_APP = "Avatar.exe";
    const string DEFAULT_APPARGS = "";
    const int DEFAULT_PORT = 23763;
    const int DEFAULT_MINRECONNECT = 20;
    const int DEFAULT_MAXRECONNECT = 3000;

    bool _bVisible = DEFAULT_VISIBLE;
    bool _bAutoConnect = DEFAULT_AUTOCONNECT;
    bool _bAutoStart = DEFAULT_AUTOSTART;
    string _sAppPath = DEFAULT_APP;
    string _sAppArgs = DEFAULT_APPARGS;
    int _nPort = DEFAULT_PORT;
    bool _bHelp = false;

    Client _client = null;
    int _nReconnectInterval = DEFAULT_MINRECONNECT;
    int _nMinReconnectInterval = DEFAULT_MINRECONNECT;
    int _nMaxReconnectInterval = DEFAULT_MAXRECONNECT;
    bool _bConnected = false;
    Timer _connectTimer = null;

    bool _bInShutdown = false;
    bool _bFirstDisconnect = true;
    bool _bWasConnected = false;

    internal Controller()
    {
      IsSingleInstance = true;
    }

    internal new void Log(string s)
    {
      if (!_bInShutdown) {
        if (MainForm != null) {
          MainForm.Invoke(_form.LogHandler, s);
        }
      }
    }

    internal void Invoke(Form1.InvokableOnFormThread fExecutable)
    {
      if (!_bInShutdown) {
        if (MainForm != null) {
          MainForm.Invoke(_form.ExecHandler, fExecutable);
        }
      }
    }

    bool TrueString(string sIn)
    {
      string s = sIn.ToLower();
      return s == "yes" || s == "true" || s == "1" || s == "on" || s == "ok" || s == "sure" || s == "yessir";
    }

    #region Begin/End // --------------------------------------

    protected override bool OnStartup(StartupEventArgs eventArgs)
    {
      var args = eventArgs.CommandLine;

      for (int i = 0; i < args.Count; ++i) {
        switch (args[i]) {

          case "-show": {
              if (args.Count > i) {
                _bVisible = TrueString(args[++i]);
              }
            }
            break;

          case "-connect": {
              if (args.Count > i) {
                _bAutoConnect = TrueString(args[++i]);
              }
            }
            break;

          case "-startapp": {
              if (args.Count > i) {
                _bAutoStart = TrueString(args[++i]);
              }
            }
            break;

          case "-app":
            if (args.Count > i) {
              _sAppPath = args[++i];
            }
            break;

          case "-args":
            if (args.Count > i) {
              _sAppArgs = args[++i];
            }
            break;

          case "-port":
            if (args.Count > i) {
              _nPort = Convert.ToInt32(args[++i]);
            }
            break;

          case "-minreconnect":
            if (args.Count > i) {
              _nMinReconnectInterval = Convert.ToInt32(args[++i]);
            }
            break;

          case "-maxreconnect":
            if (args.Count > i) {
              _nMaxReconnectInterval = Convert.ToInt32(args[++i]);
            }
            break;

          case "--help":
          case "-help":
          case "-?":
            _bHelp = true;
            break;
          default: break;
        }
      }

      return true;
    }

    protected override void OnCreateMainForm()
    {
      string sBaseFolder = AppDomain.CurrentDomain.BaseDirectory;
      Directory.SetCurrentDirectory(sBaseFolder);
      
      _form = new Form1(this);

      if (_bHelp) {
        _bVisible = true;
        _bAutoConnect = false;
        _bAutoStart = false;
      }

      MainForm = _form;

      _form.SetVisible(_bVisible);
      _form.SetConnected(_bConnected);

      _nReconnectInterval = _nMinReconnectInterval;
    }

    internal void Start()
    {
      _form.SetConnecting(_bAutoConnect);

      if (_bAutoConnect) {
        StartConnectTimer();
      }

      if (_bHelp) {
        string sHelp = @"Controls an application program via TCP/SRPC
-help\n
-show # Show dialog, default:no\n
-connect # Connect automatically, default:yes\n
-startapp # Start app automatically, default:yes\n
-app <path to application program> # default:" + DEFAULT_APP + @"\n
-args <program arguments> # default:<empty>\n
-port <program TCP port> # default:" + DEFAULT_PORT + @"\n
-minreconnect <min reconnect timer> # in msec, default:" + DEFAULT_MINRECONNECT + @"\n
-maxreconnect <max reconnect timer> # in msec, default:" + DEFAULT_MAXRECONNECT + @"\n
";
        _form.SetText(sHelp.Replace("\n", Environment.NewLine));
      }
    }

    internal void Stop()
    {
      if (!_bInShutdown) {
        _bInShutdown = true;

        if (_client != null) {
          _client.Disconnect();
        }
      }
    }

    protected override void OnShutdown()
    {
      if (!_bInShutdown) {
        Stop();
      }
    }

    #endregion

    #region Main Flow // --------------------------------------

    void StartConnectTimer()
    {
      _nReconnectInterval *= 2;
      if (_nReconnectInterval > _nMaxReconnectInterval) {
        _nReconnectInterval = _nMaxReconnectInterval;
      }

      _connectTimer = new Timer();
      _connectTimer.Tick += new EventHandler(OnConnectTimer);
      _connectTimer.Interval = _nReconnectInterval;
      _connectTimer.Start();
    }

    void StopConnectTimer()
    {
      _connectTimer.Stop();
      _connectTimer.Dispose();
      _connectTimer = null;
    }

    void OnConnectTimer(object sender, EventArgs e)
    {
      StopConnectTimer();
      Connect();
    }

    void Connect()
    {
      if (_client == null) {
        _bConnected = false;

        _client = new Client(this);
        _client.Connect(_nPort, OnConnected, OnMessage, OnDisconnected);
      }
    }

    void Disconnect()
    {
      _client.Disconnect();
      _client = null;
    }

    #endregion

    #region Networking events // -----------------------------------

    void OnConnected()
    {
      _bConnected = true;
      _bWasConnected = true;
      _nReconnectInterval = _nMinReconnectInterval;

      Invoke(() => OnConnectedMainThread());
    }

    void OnConnectedMainThread()
    {
      _form.SetConnected(_bConnected);

      var srpc = new Srpc.Message();
      srpc.Set(Srpc.Key.Method, "TrayIcon_Hello");
      Send(srpc);
    }

    void OnMessage(ref Srpc.Message request, ref Srpc.Message response)
    {
      string sMethod = request.GetString(Srpc.Key.Method);
      switch (sMethod) {
        case "TrayIcon_ConnectionStatus": {
          int nConnections = request.GetInt("nConnections");
          if (nConnections > 0) {
            Invoke(() => _form.ShowConnectedAppStatus());
          } else {
            Invoke(() => _form.ShowDisconnectedAppStatus());
          }
        } break;
      }
    }

    void OnDisconnected()
    {
      _bConnected = false;
      _client = null;

      Invoke(() => OnDisconnectedMainThread());
    }

    void OnDisconnectedMainThread()
    {
      _form.SetConnected(_bConnected);

      if (_bInShutdown) {
        Log("OnDisconnected: skipping reconnect in shutdown");
      } else {
        if (_bFirstDisconnect) {
          _bFirstDisconnect = false;
          if (!_bWasConnected) {
            if (_bAutoStart) {
              LaunchApp();
            }
          }
        }

        StartConnectTimer();
      }
    }

    void Send(Srpc.Message srpc) { Send(srpc, null); }
    void Send(Srpc.Message srpc, Client.SrpcCompletion fComplete)
    {
      if (_bConnected) {
        if (_client != null) {
          _client.Send(srpc, fComplete);
        }
      }
    }

    #endregion

    #region Commands // ---------------------------------------

    internal void DoConnect()
    {
      _bAutoConnect = true;
      _bFirstDisconnect = false;
      _form.SetConnecting(_bAutoConnect);
      StartConnectTimer();
    }

    internal void LaunchApp()
    {
      Log("Launching " + _sAppPath + " " + _sAppArgs);
      var process = Process.Start(_sAppPath, _sAppArgs);
      if (process == null) {
        Log("Process.Start failed");
      } else {
        _nReconnectInterval = _nMinReconnectInterval;
      }
    }

    #endregion
  
    #region Send Apollo messages // ---------------------------------------

    internal void TerminateApp()
    {
      var srpc = new Srpc.Message();
      srpc.Set(Srpc.Key.Method, "MainLoop_EndLoop");
      Send(srpc);
    }

    #endregion
  
  }
}
