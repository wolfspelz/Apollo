using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace OVW
{
  class Client
  {
    internal delegate void SrpcCompletion(Srpc.Message response);

    Controller Controller;

    ConnectedCallback _fOnConnected = null;
    DisconnectedCallback _fOnDisconnected = null;

    const int BUFFER_SIZE = 10240;
    Socket _socket = null;
    byte[] _bytes = new byte[BUFFER_SIZE];
    int _nSize = BUFFER_SIZE;
    bool _bReceiving = false;
    string _sBuffer = "";
    int _nSrpcId = 1;
    Dictionary<string, SrpcCompletion> _completions = new Dictionary<string, SrpcCompletion>();

    internal Client(Controller c)
    {
      Controller = c;
    }

    internal void Log(string s)
    {
      Controller.Log(s);
    }

    #region Control // -------------------------------

    internal delegate void ConnectedCallback();
    internal delegate void DisconnectedCallback();

    internal void Connect(ConnectedCallback fOnConnected, DisconnectedCallback fOnDisconnected)
    {
      _fOnConnected = fOnConnected;
      _fOnDisconnected = fOnDisconnected;

      Log("Connecting...");

      Socket newSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

      int nPort = Controller.GetRegistryValue(Globals.Registry.Port, 23763);

      IPEndPoint iep = new IPEndPoint(IPAddress.Loopback, nPort);
      newSocket.BeginConnect(iep, new AsyncCallback(OnConnected), newSocket);
    }

    internal void Disconnect()
    {
      if (_socket != null) {
        var socket = _socket;
        _socket = null;
        socket.Close();

        Log("Disconnected");
      }
    }

    internal void Send(string sText)
    {
      byte[] message = Encoding.UTF8.GetBytes(sText);
      lock (this) {
        Log("OUT " + sText);
        _socket.BeginSend(message, 0, message.Length, SocketFlags.None, new AsyncCallback(OnSent), _socket);
      }
    }

    internal void Send(Srpc.Message msg, SrpcCompletion fComplete)
    {
      if (fComplete != null) {
        string sSrpcId = "_" + _nSrpcId++;
        msg.Set("SrpcId", sSrpcId);
        _completions.Add(sSrpcId, fComplete);
      }

      Send(msg.ToString() + "\n");
    }

    internal void Send(Srpc.Message msg)
    {
      Send(msg, null);
    }

    #endregion

    #region Socket management // -------------------------------

    void Receive()
    {
      lock (this) {
        if (!_bReceiving) {
          if (_socket != null) {
            _bReceiving = true;
            _socket.BeginReceive(_bytes, 0, _nSize, SocketFlags.None, new AsyncCallback(OnReceived), _socket);
          }
        }
      }
    }

    void OnConnected(IAsyncResult iar)
    {
      _socket = (Socket) iar.AsyncState;

      try {
        _socket.EndConnect(iar);

        Log("Connected to: " + _socket.RemoteEndPoint.ToString());

        if (_fOnConnected != null) {
          _fOnConnected();
        }

        Receive();
      } catch (SocketException ex) {
        Log("Error connecting: " + ex.Message);

        if (_fOnDisconnected != null) {
          _fOnDisconnected();
        }
      }
    }

    void OnReceived(IAsyncResult iar)
    {
      //Socket remoteSocket = (Socket)iar.AsyncState; // disposed

      lock (this) {
        if (_socket == null) {
          OnDisconnected();
        } else {
          _bReceiving = false;
          int nBytesRead = 0;
          try {
            nBytesRead = _socket.EndReceive(iar);
          } catch { }

          if (nBytesRead == 0) {
            OnDisconnected();
          } else {
            string sData = Encoding.UTF8.GetString(_bytes, 0, nBytesRead);
            Log("IN " + sData);
            _sBuffer += sData;

            Srpc.Message srpc = null;
            do {
              srpc = ProcessData(ref _sBuffer);
              if (srpc != null) {
                string sSrpcId = srpc.GetString("SrpcId");
                if (!String.IsNullOrEmpty(sSrpcId)) {
                  HandleResponse(srpc, sSrpcId);
                } else {
                  HandleRequest(srpc);
                }
              }
            } while (srpc != null);

            Receive();
          }
        }
      }
    }

    private Srpc.Message ProcessData(ref string sData)
    {
      Srpc.Message msg = null;

      string sLfClean = sData.Replace("\r\n", "\n");
      int nEnd = sLfClean.IndexOf("\n\n");
      if (nEnd >= 0) {
        string sMessage = sLfClean.Substring(0, nEnd + 1);
        sData = sLfClean.Substring(nEnd + 2);
        msg = new Srpc.Message(sMessage);
      }

      return msg;
    }

    private void HandleResponse(Srpc.Message response, string sSrpcId)
    {
      if (_completions.ContainsKey(sSrpcId)) {
        var fCompletion = _completions[sSrpcId];
        if (fCompletion != null) {
          fCompletion(response);
        }
        _completions.Remove(sSrpcId);
      }
    }

    private void HandleRequest(Srpc.Message request)
    {
    }

    void OnSent(IAsyncResult iar)
    {
      lock (this) {
        Socket remoteSocket = (Socket) iar.AsyncState;
        int nSent = remoteSocket.EndSend(iar);
      }

      Receive();
    }

    void OnDisconnected()
    {
      Disconnect();

      if (_fOnDisconnected != null) {
        _fOnDisconnected();
      }
    }

    #endregion
  }
}
