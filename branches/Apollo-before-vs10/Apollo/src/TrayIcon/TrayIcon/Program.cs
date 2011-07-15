using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace TrayIcon
{
  static class Program
  {
    static Controller _app = null;

    [STAThread]
    static void Main(string[] args)
    {
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);

      var c = new Controller();

      //Application.Run(new Form1());
      try {
        _app = new Controller();
        _app.Run(args);
      } catch {
        // Something about a system resource required for the single instance app 
        // is not available, whatever, we ignore it and let us restart later again.
      }
    }
  }
}
