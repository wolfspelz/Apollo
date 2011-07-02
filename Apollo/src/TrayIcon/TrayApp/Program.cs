using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace TrayApp
{
  static class Program
  {
    static Controller _app = null;

    [STAThread]
    static void Main(string[] args)
    {
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);

      string sFile = Process.GetCurrentProcess().MainModule.FileName;
      string sPath = Path.GetDirectoryName(sFile);
      string sCwd = Directory.GetCurrentDirectory();
      if (sPath != sCwd) {
        Directory.SetCurrentDirectory(sPath);
      }

      var c = new Controller();

      //Application.Run(new Form1());
      try {
        _app = new Controller();
        _app.Run(args);
      } catch (Exception ex) {
        var ex1 = ex;
        // Something about a system resource required for the single instance app 
        // is not available, whatever, we ignore it and let us restart later again.
      }
    }
  }
}
