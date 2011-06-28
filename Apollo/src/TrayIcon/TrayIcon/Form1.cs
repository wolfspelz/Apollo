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

    public delegate void InvokableOnFormThread();
    public delegate void ExecDelegate(InvokableOnFormThread fExecutable);
    public ExecDelegate ExecHandler;

    bool _bVisible = false;
    bool _bFirstShown = true;
    bool _bConnected = false;
    bool _bConnecting = false;

    MenuItem _itemStart;
    MenuItem _itemStop;
    MenuItem _itemShow;

    private System.Windows.Forms.ContextMenu _contextMenu;
    
    public Form1(Controller c)
    {
      Controller = c;

      InitializeComponent();

      LogHandler = new LogDelegate(Log);
      ExecHandler = new ExecDelegate(Exec);

      _contextMenu = new System.Windows.Forms.ContextMenu();
      _itemStart = _contextMenu.MenuItems.Add("Start Avatar", new System.EventHandler(_menuStart_Click));
      _itemStop = _contextMenu.MenuItems.Add("Stop Avatar", new System.EventHandler(_menuStop_Click));
      _contextMenu.MenuItems.Add("-");
      _itemShow = _contextMenu.MenuItems.Add("Show", new System.EventHandler(_menuShow_Click));
      _contextMenu.MenuItems.Add("E&xit", new System.EventHandler(_menuExit_Click));

      notifyIcon1.ContextMenu = _contextMenu;
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

    internal void Exec(InvokableOnFormThread fExecutable)
    {
      fExecutable();
    }

    internal void SetVisible(bool bVisible)
    {
      _bVisible = bVisible;

      if (_bVisible) {

        _itemShow.Enabled = false;

        ShowInTaskbar = true;
        Opacity = 1.0;
        Show();

      } else {

        _itemShow.Enabled = true;

        ShowInTaskbar = false;
        Opacity = 0.0;
        Hide();

      }
    }

    internal void SetConnecting(bool bConnecting)
    {
      _bConnecting = bConnecting;
      if (_bConnecting) {
        buttonConnect.Enabled = false;
      } else {
        buttonConnect.Enabled = true;
      }
    }

    internal void SetConnected(bool bConnected)
    {
      _bConnected = bConnected;

      if (_bConnected) {

        _itemStart.Enabled = false;
        _itemStop.Enabled = true;
        ShowUnknownAppStatus();

      } else {

        _itemStart.Enabled = true;
        _itemStop.Enabled = false;
        ShowUnknownAppStatus();

      }
    }

    internal void ShowUnknownAppStatus()
    {
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
      notifyIcon1.Icon = ((System.Drawing.Icon) (resources.GetObject("IconUnknown")));
    }

    internal void ShowConnectedAppStatus()
    {
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
      notifyIcon1.Icon = ((System.Drawing.Icon) (resources.GetObject("IconConnected")));
    }

    internal void ShowDisconnectedAppStatus()
    {
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
      notifyIcon1.Icon = ((System.Drawing.Icon) (resources.GetObject("IconDisconnected")));
    }

    private void Form1_VisibleChanged(object sender, EventArgs e)
    {
      if (!_bVisible) {
        Visible = false;
      }
    }

    private void _menuExit_Click(object Sender, EventArgs e)
    {
      // If visible, then close does hide (see FormClosing event)
      // Make invisible so, that close does quit
      SetVisible(false);
      Close();
    }

    private void _menuShow_Click(object Sender, EventArgs e)
    {
      SetVisible(true);
    }

    private void _menuStart_Click(object Sender, EventArgs e)
    {
      Controller.LaunchApp();
    }

    private void _menuStop_Click(object Sender, EventArgs e)
    {
      Controller.TerminateApp();
    }

    private void buttonConnect_Click(object sender, EventArgs e)
    {
      Controller.DoConnect();
    }

    private void buttonClear_Click(object sender, EventArgs e)
    {
      textBoxLog.Clear();
    }

    private void Form1_Shown(object sender, EventArgs e)
    {
      if (_bFirstShown) {
        _bFirstShown = false;
        Controller.Start();
      }
    }

    private void Form1_FormClosing(object sender, FormClosingEventArgs e)
    {
      if (_bVisible) {
        // User close dialog [x]
        e.Cancel = true;
        SetVisible(false);
      } else {
        Controller.Stop();
      }
    }

    private void notifyIcon1_MouseClick(object sender, MouseEventArgs e)
    {
      //_contextMenu.Show(notifyIcon1, e.Location);
    }

    private void notifyIcon1_MouseDoubleClick(object sender, MouseEventArgs e)
    {

    }

  }
}
