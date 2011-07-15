using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;
using System.Linq;
using System.IO;

namespace OVW
{
  [RunInstaller(true)]
  public partial class clsRegisterDll : System.Configuration.Install.Installer
  {
    public clsRegisterDll()
    {
      InitializeComponent();
    }

    [System.Security.Permissions.SecurityPermission(System.Security.Permissions.SecurityAction.Demand)]
    public override void Commit(System.Collections.IDictionary savedState)
    {
      base.Commit(savedState);

      string regasmPath = System.Runtime.InteropServices.RuntimeEnvironment.GetRuntimeDirectory() + @"regasm.exe";
      string componentPath = typeof(clsRegisterDll).Assembly.Location;
      string componentFolder = Path.GetDirectoryName(componentPath);

      var startInfo = new System.Diagnostics.ProcessStartInfo(regasmPath, "/codebase \"" + componentPath + "\"");
      startInfo.WorkingDirectory = Path.GetDirectoryName(componentFolder);
      System.Diagnostics.Process.Start(startInfo);
    }

    [System.Security.Permissions.SecurityPermission(System.Security.Permissions.SecurityAction.Demand)]
    public override void Install(System.Collections.IDictionary stateSaver)
    {
      base.Install(stateSaver);
    }

    [System.Security.Permissions.SecurityPermission(System.Security.Permissions.SecurityAction.Demand)]
    public override void Uninstall(System.Collections.IDictionary savedState)
    {
      string regasmPath = System.Runtime.InteropServices.RuntimeEnvironment.GetRuntimeDirectory() + @"regasm.exe";
      string componentPath = typeof(clsRegisterDll).Assembly.Location;
      string componentFolder = Path.GetDirectoryName(componentPath);

      var startInfo = new System.Diagnostics.ProcessStartInfo(regasmPath, "/unregister \"" + componentPath + "\"");
      startInfo.WorkingDirectory = Path.GetDirectoryName(componentFolder);
      var process = System.Diagnostics.Process.Start(startInfo);
      process.WaitForExit(10000);

      base.Uninstall(savedState);
    }

  }
}
