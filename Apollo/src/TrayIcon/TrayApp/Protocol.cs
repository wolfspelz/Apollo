using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TrayApp
{
  internal class Protocol
  {
    internal class Hello
    {
      internal const string Method = "TrayIcon_Hello";
    }

    internal class ConnectionStatus
    {
      internal const string Method = "TrayIcon_ConnectionStatus";
      internal class Key
      {
        internal const string Connections = "nConnections";
      }
    }

  }
}
