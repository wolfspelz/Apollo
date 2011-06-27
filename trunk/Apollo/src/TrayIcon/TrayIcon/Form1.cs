using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace TrayIcon
{
  public partial class Form1 : Form
  {
    Controller Controller { get; set; }

    public delegate void LogDelegate(string s);
    public LogDelegate LogHandler;

    public delegate void FormThreadInvokable();
    public delegate void ExecDelegate(FormThreadInvokable fExecutable);
    public ExecDelegate ExecHandler;

    bool _bVisible = false;

    private System.Windows.Forms.ContextMenu _contextMenu;
    
    public Form1(Controller c)
    {
      Controller = c;

      InitializeComponent();

      LogHandler = new LogDelegate(Log);
      ExecHandler = new ExecDelegate(Exec);

      _contextMenu = new System.Windows.Forms.ContextMenu();
      _contextMenu.MenuItems.Add("Start Avatar", new System.EventHandler(this._menuStart_Click));
      _contextMenu.MenuItems.Add("Stop Avatar", new System.EventHandler(this._menuStop_Click));
      _contextMenu.MenuItems.Add("-");
      _contextMenu.MenuItems.Add("E&xit", new System.EventHandler(this._menuExit_Click));

      notifyIcon1.ContextMenu = this._contextMenu;
    }

    internal void Log(string s)
    {
      try {
        if (textBoxLog.TextLength > 20000) {
          textBoxLog.Text = textBoxLog.Text.Substring(textBoxLog.TextLength - 10000);
        }
        textBoxLog.AppendText(s.Replace("\r\n", "\n").Replace("\n", Environment.NewLine).Replace("\n", "\\n") + Environment.NewLine);
        textBoxLog.SelectionStart = textBoxLog.TextLength;
        textBoxLog.ScrollToCaret();
      } catch { }
    }

    internal void SetText(string s)
    {
      try {
        textBoxLog.Text = s;
        textBoxLog.SelectionStart = textBoxLog.TextLength;
        textBoxLog.SelectionLength = 0;
      } catch { }
    }

    internal void Exec(FormThreadInvokable fExecutable)
    {
      fExecutable();
    }

    internal void SetVisible(bool bVisible)
    {
      _bVisible = bVisible;

      if (_bVisible) {
        ShowInTaskbar = true;
        Opacity = 1.0;
      } else {
        ShowInTaskbar = false;
        Opacity = 0.0;
      }
    }

    private void Form1_VisibleChanged(object sender, EventArgs e)
    {
      if (!_bVisible) {
        Visible = false;
      }
    }

    private void _menuExit_Click(object Sender, EventArgs e)
    {
      this.Close();
    }

    private void _menuStart_Click(object Sender, EventArgs e)
    {
      Controller.StartExe();
    }

    private void _menuStop_Click(object Sender, EventArgs e)
    {
      Controller.SendQuit();
    }

    private void notifyIcon1_MouseClick(object sender, MouseEventArgs e)
    {
      //this._contextMenu.Show(notifyIcon1, e.Location);
    }

    private void buttonConnect_Click(object sender, EventArgs e)
    {
      Controller.GoOnline();
    }

    private void buttonClear_Click(object sender, EventArgs e)
    {
      textBoxLog.Clear();
    }

  }
}
