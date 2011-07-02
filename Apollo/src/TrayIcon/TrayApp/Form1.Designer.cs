namespace TrayApp
{
  partial class Form1
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null)) {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
      this.notifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
      this.textBoxLog = new System.Windows.Forms.TextBox();
      this.buttonConnect = new System.Windows.Forms.Button();
      this.buttonClear = new System.Windows.Forms.Button();
      this.timerDblClick = new System.Windows.Forms.Timer(this.components);
      this.buttonStatus = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // notifyIcon1
      // 
      this.notifyIcon1.Icon = ((System.Drawing.Icon) (resources.GetObject("notifyIcon1.Icon")));
      this.notifyIcon1.Text = "Avatar";
      this.notifyIcon1.Visible = true;
      this.notifyIcon1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.notifyIcon1_MouseClick);
      this.notifyIcon1.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.notifyIcon1_MouseDoubleClick);
      // 
      // textBoxLog
      // 
      this.textBoxLog.Anchor = ((System.Windows.Forms.AnchorStyles) ((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.textBoxLog.Location = new System.Drawing.Point(13, 42);
      this.textBoxLog.Multiline = true;
      this.textBoxLog.Name = "textBoxLog";
      this.textBoxLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.textBoxLog.Size = new System.Drawing.Size(544, 237);
      this.textBoxLog.TabIndex = 0;
      // 
      // buttonConnect
      // 
      this.buttonConnect.Location = new System.Drawing.Point(13, 13);
      this.buttonConnect.Name = "buttonConnect";
      this.buttonConnect.Size = new System.Drawing.Size(98, 23);
      this.buttonConnect.TabIndex = 1;
      this.buttonConnect.Text = "Start Connecting";
      this.buttonConnect.UseVisualStyleBackColor = true;
      this.buttonConnect.Click += new System.EventHandler(this.buttonConnect_Click);
      // 
      // buttonClear
      // 
      this.buttonClear.Anchor = ((System.Windows.Forms.AnchorStyles) ((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonClear.Location = new System.Drawing.Point(482, 13);
      this.buttonClear.Name = "buttonClear";
      this.buttonClear.Size = new System.Drawing.Size(75, 23);
      this.buttonClear.TabIndex = 2;
      this.buttonClear.Text = "Clear";
      this.buttonClear.UseVisualStyleBackColor = true;
      this.buttonClear.Click += new System.EventHandler(this.buttonClear_Click);
      // 
      // timerDblClick
      // 
      this.timerDblClick.Tick += new System.EventHandler(this.timerDblClick_Tick);
      // 
      // buttonStatus
      // 
      this.buttonStatus.Location = new System.Drawing.Point(118, 13);
      this.buttonStatus.Name = "buttonStatus";
      this.buttonStatus.Size = new System.Drawing.Size(75, 23);
      this.buttonStatus.TabIndex = 3;
      this.buttonStatus.Text = "Status";
      this.buttonStatus.UseVisualStyleBackColor = true;
      this.buttonStatus.Click += new System.EventHandler(this.buttonStatus_Click);
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(569, 291);
      this.Controls.Add(this.buttonStatus);
      this.Controls.Add(this.buttonClear);
      this.Controls.Add(this.buttonConnect);
      this.Controls.Add(this.textBoxLog);
      this.Icon = ((System.Drawing.Icon) (resources.GetObject("$this.Icon")));
      this.Name = "Form1";
      this.ShowInTaskbar = false;
      this.Text = "Avatar Tray Icon";
      this.Shown += new System.EventHandler(this.Form1_Shown);
      this.VisibleChanged += new System.EventHandler(this.Form1_VisibleChanged);
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.NotifyIcon notifyIcon1;
    private System.Windows.Forms.TextBox textBoxLog;
    private System.Windows.Forms.Button buttonConnect;
    private System.Windows.Forms.Button buttonClear;
    private System.Windows.Forms.Timer timerDblClick;
    private System.Windows.Forms.Button buttonStatus;
  }
}

