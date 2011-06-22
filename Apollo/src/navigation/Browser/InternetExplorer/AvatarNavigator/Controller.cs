using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Win32;
using System.IO;

namespace SpicIE.AvatarNavigator
{
    public class Controller
    {
        bool _bAutoStart = true;
        string _sId = "";
        Client _client = null;
        Random _rnd = new Random();
        string _sUrl = "";
        int _nReconnectInterval = 20;
        int _nMinReconnectInterval = 20;
        int _nMaxReconnectInterval = 10000;
        System.Threading.Timer _connectTimer = null;
        bool _bInShutdown = false;
        string _sContextHandle = "";

        internal void Log(string s)
        {
            if (!_bInShutdown)
            {
                try
                {
                    using (StreamWriter sw = File.AppendText("C:\\logfile.txt"))
                    {
                        sw.WriteLine(s.Replace("\r\n", "\n").Replace("\n", "\\n"));
                        sw.Close();
                    }
                }
                catch { }
            }
        }

        internal void Run()
        {
            if (String.IsNullOrEmpty(_sId))
            {
                _sId = this.GetHashCode() + "-" + DateTime.Now.Ticks + "-" + GetRandomNumber(1000000000);
            }

            _nMinReconnectInterval = GetRegistryValue("AvatarNavigator/MinReconnectInterval", 20);
            _nMaxReconnectInterval = GetRegistryValue("AvatarNavigator/MaxReconnectInterval", 10000);

            if (_bAutoStart)
            {
                _nReconnectInterval = _nMinReconnectInterval;
                StartConnectTimer();
            }
        }

        void StartConnectTimer()
        {
            lock (this)
            {
                if (_connectTimer == null)
                {
                    _nReconnectInterval *= 2;
                    if (_nReconnectInterval > _nMaxReconnectInterval)
                    {
                        _nReconnectInterval = _nMaxReconnectInterval;
                    }
                    _connectTimer = new System.Threading.Timer(OnConnectTimer, null, _nReconnectInterval, System.Threading.Timeout.Infinite);
                }
            }
        }

        void OnConnectTimer(object state)
        {
            lock (this)
            {
                _connectTimer = null;
                Connect();
            }
        }

        internal void OnShutdown()
        {
            _bInShutdown = true;
            if (_client != null)
            {
                _client.Disconnect();
            }
        }

        internal void Connect()
        {
            if (_client == null)
            {
                _client = new Client(this);
                _client.SetQueueing(true);
                _client.Connect(OnConnected, OnDisconnected);
            }
        }

        internal void Disconnect()
        {
            _client.Disconnect();
            _client = null;
        }

        internal void OnGetHandle(Srpc.Message response)
        {
            _sContextHandle = response.GetString("h");
            if (_client != null)
            {
                _client.SetQueueing(false);
            }
        }

        internal void OnHello(Srpc.Message response)
        {
            var request = new Srpc.Message();
            request.Set(Srpc.Key.Method, "System_GetHandle");
            _client.Request(request, OnGetHandle);
        }

        internal void OnConnected()
        {
            _nReconnectInterval = _nMinReconnectInterval;

            if (_client != null)
            {
                var request = new Srpc.Message();
                request.Set(Srpc.Key.Method, "Navigation_NavigatorHello");
                _client.Request(request, OnHello);
            }
        }

        internal void OnDisconnected()
        {
            _client = null;

            if (_bInShutdown)
            {
                Log("OnDisconnected: skipping reconnect in shutdown");
            }
            else
            {
                StartConnectTimer();
            }
        }

        internal void SendText(string sData)
        {
            var srpc = new Srpc.Message();
            srpc.Set(Srpc.Key.Method, "Data");
            srpc.Set("Data", sData);
            if (_client != null)
            {
                _client.Send(srpc);
            }
        }

        internal void SendNavigate(string sUrl)
        {
            var srpc = new Srpc.Message();
            srpc.Set(Srpc.Key.Method, "Navigate");
            srpc.Set("Url", sUrl);
            if (_client != null)
            {
                _client.Send(srpc);
            }
        }

        internal int GetRandomNumber(int nMax)
        {
            int nRandom = 0;
            lock (this)
            {
                nRandom = _rnd.Next(nMax);
            }
            return nRandom;
        }

        internal string GetRegistryValue(string sName, string sDefault)
        {
            string sResult = "";
            try
            {
                using (var key = Registry.CurrentUser.OpenSubKey(@"Software\OpenVirtualWorld\Avatar"))
                {
                    if (key != null)
                    {
                        sResult = (string)key.GetValue(sName);
                    }
                }
            }
            catch { }
            return sResult;
        }

        internal int GetRegistryValue(string sName, int nDefault)
        {
            int nResult = 0;
            if (!Int32.TryParse(GetRegistryValue(sName, ""), out nResult))
            {
                nResult = nDefault;
            }
            return nResult;
        }

        internal void OnTabCreate()
        {
            Log("OnTabCreate " + _sId + " " + _sUrl);
        }

        internal void OnTabDestroy()
        {
            Log("OnTabDestroy " + _sId + " " + _sUrl);
        }

        internal void OnTabShow()
        {
            Log("OnTabShow " + _sId + " " + _sUrl);
            lock (this)
            {
                if (_client != null)
                {
                    var srpc = new Srpc.Message();
                    srpc.Set(Srpc.Key.Method, "Show");
                    _client.Send(srpc);
                }
            }
        }

        internal void OnTabHide()
        {
            Log("OnTabHide " + _sId + " " + _sUrl);
            lock (this)
            {
                if (_client != null)
                {
                    var srpc = new Srpc.Message();
                    srpc.Set(Srpc.Key.Method, "Hide");
                    _client.Send(srpc);
                }
            }
        }

        internal void OnTabNavigate(string sUrl)
        {
            _sUrl = sUrl;
            Log("OnTabNavigate " + _sId + " " + _sUrl);
            lock (this)
            {
                if (_client != null)
                {
                    var srpc = new Srpc.Message();
                    srpc.Set(Srpc.Key.Method, "Navigate");
                    srpc.Set("Url", _sUrl);
                    _client.Send(srpc);
                }
            }
        }
    
    }
}
