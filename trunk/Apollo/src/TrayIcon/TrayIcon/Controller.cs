using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Microsoft.VisualBasic.ApplicationServices;
using System.Diagnostics;

namespace TrayIcon
{
  public class Controller : WindowsFormsApplicationBase
  {
    Form1 _form = null;
    bool _bInShutdown = false;

    const string DEFAULT_EXE = "Avatar.exe";
    const string DEFAULT_EXEARGS = "";
    const int DEFAULT_PORT = 23763;
    const int DEFAULT_MINRECONNECT = 20;
    const int DEFAULT_MAXRECONNECT = 3000;

    bool _bVisible = false;
    bool _bAutoStart = true;
    string _sExePath = DEFAULT_EXE;
    string _sExeArgs = DEFAULT_EXEARGS;
    int _nPort = DEFAULT_PORT;
    bool _bHelp = false;

    Client _client = null;
    int _nReconnectInterval = DEFAULT_MINRECONNECT;
    int _nMinReconnectInterval = DEFAULT_MINRECONNECT;
    int _nMaxReconnectInterval = DEFAULT_MAXRECONNECT;
    bool _bConnected = false;
    Timer _connectTimer = null;

    internal Controller()
    {
      this.IsSingleInstance = true;
    }

    internal new void Log(string s)
    {
      if (!_bInShutdown) {
        if (this.MainForm != null && this.MainForm.Visible) {
          this.MainForm.Invoke(_form.LogHandler, s);
        }
      }
    }

    internal void Invoke(Form1.FormThreadInvokable fExecutable)
    {
      if (this.MainForm != null) {
        this.MainForm.Invoke(_form.ExecHandler, fExecutable);
      }
    }

    #region Begin/End // --------------------------------------

    protected override bool OnStartup(StartupEventArgs eventArgs)
    {
      var args = eventArgs.CommandLine;

      for (int i = 0; i < args.Count; ++i) {
        switch (args[i]) {

          case "-show": {
              _bVisible = true;
            }
            break;

          case "-autostart": {
              _bAutoStart = true;
            }
            break;

          case "-exe":
            if (args.Count > i) {
              _sExePath = args[++i];
            }
            break;

          case "-exeargs":
            if (args.Count > i) {
              _sExeArgs = args[++i];
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
      _form = new Form1(this);

      if (_bHelp) {
        _bVisible = true;
        _bAutoStart = false;
      }

      if (!_bVisible) {
        _form.ShowInTaskbar = false;
        _form.ShowIcon = false;
        _form.Opacity = 0.0;
        //_form.Hide(); // does not work
      }

      _form.SetVisible(_bVisible);

      this.MainForm = _form;

      _nReconnectInterval = _nMinReconnectInterval;

      if (_bAutoStart) {
        StartConnectTimer();
      }

      if (_bHelp) {
        string sHelp = @"-help\n
-show # Show dialog, default:no\n
-autostart # Connect automatically, default:yes\n
-exe <path to program exe> # default:" + DEFAULT_EXE + @"\n
-exeargs <program arguments> # default:(empty)\n
-port <program TCP port> # default:" + DEFAULT_PORT + @"\n
-minreconnect <min reconnect timer in msec> # default:" + DEFAULT_MINRECONNECT + @"\n
-maxreconnect <max reconnect timer in msec> # default:" + DEFAULT_MAXRECONNECT + @"\n
";
        _form.SetText(sHelp.Replace("\n", Environment.NewLine));
      }
    }

    protected override void OnShutdown()
    {
      _bInShutdown = true;
      if (_client != null) {
        _client.Disconnect();
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
        _client.Connect(_nPort, OnConnected, OnDisconnected);
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
      _nReconnectInterval = _nMinReconnectInterval;
    }

    void OnDisconnected()
    {
      _bConnected = false;
      _client = null;

      if (_bInShutdown) {
        Log("OnDisconnected: skipping reconnect in shutdown");
      } else {
        Invoke(() => StartConnectTimer());
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

    internal void GoOnline()
    {
      StartConnectTimer();
    }

    internal void StartExe()
    {
      var process = Process.Start(_sExePath, _sExeArgs);
      if (process == null) {
        Log("Process.Start failed");
      } else {
        _nReconnectInterval = _nMinReconnectInterval;
      }
    }

    #endregion
  
    #region Send Apollo messages // ---------------------------------------

    internal void SendQuit()
    {
      var srpc = new Srpc.Message();
      srpc.Set(Srpc.Key.Method, "MainLoop_EndLoop");
      Send(srpc);
    }

    #endregion
  
  }
}
