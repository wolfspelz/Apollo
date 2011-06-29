using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace TrayIcon
{
  class Babelfish
  {
    public string FilePrefix { get; set; }
    public string FileSuffix { get; set; }
    public string Language { get; set; }

    Dictionary<string, string> _dict = null;


    private static Babelfish _instance = null;
    public static Babelfish Instance
    {
      get
      {
        if (_instance == null) {
          _instance = new Babelfish();
        }
        return _instance;
      }
    }

    public static string Translate(string sText)
    {
      return Instance._Translate(sText);
    }

    Babelfish()
    {
      FilePrefix = "Babelfish_";
      FileSuffix = ".txt";
      Language = "";
    }

    public void Load()
    {
      _dict = new Dictionary<string, string>();

      string sLang = Language;
      if (String.IsNullOrEmpty(sLang)) {
        sLang = System.Threading.Thread.CurrentThread.CurrentCulture.Name;
      }
      
      string sFilename = FilePrefix + sLang + FileSuffix;
      if (!File.Exists(sFilename)) {
        var parts = sLang.Split('-');
        sLang = parts[0];
        sFilename = FilePrefix + sLang + FileSuffix;
        if (!File.Exists(sFilename)) {
          sFilename = FilePrefix + "en" + FileSuffix;
        }
      }

      if (File.Exists(sFilename)) {
        var lines = File.ReadAllLines(sFilename, Encoding.UTF8);
        foreach (string sLine in lines) {
          var parts = sLine.Split('=');
          if (parts.Length == 2) {
            _dict[parts[0]] = parts[1];
          } else if (parts.Length == 1) {
            _dict[parts[0]] = "";
          }
        }
      }
    }

    public string _Translate(string sText)
    {
      string sTranslated = null;

      if (_dict == null) {
        Load();
      }

      if (_dict.ContainsKey(sText)) {
        sTranslated = _dict[sText];
      }

      if (sTranslated == null) {
        sTranslated = sText;
      }

      return sTranslated;
    }

  }
}
