using System;
using System.Collections.Generic;
using System.Text;
using System.Globalization;

namespace SpicIE.AvatarNavigator
{
    public class Srpc
    {
        public delegate void Handler(ref Request request, ref Response response);

        public class HttpUnauthorizedException : Exception { }
        public class HttpRedirectException : Exception { public HttpRedirectException(string sUrl) : base(sUrl) { } }
        public class HistoryBackException : Exception { }
        public class UnauthorizedException : Exception { public UnauthorizedException(string sMessage) : base(sMessage) { } }
        public class MissingParameterException : Exception { public MissingParameterException(string sKey) : base("Missing parameter: " + sKey) { } }

        public class SrpcException : Exception
        {
            Srpc.Message _srpcMessage;
            public Srpc.Message Srpc { get { return _srpcMessage; } }

            public SrpcException(Srpc.Message msg)
            {
                _srpcMessage = msg;
            }
        }

        public static string Translate(string source, List<KeyValuePair<string, string>> replaces)
        {
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < source.Length; i++)
            {
                bool anyFound = false;
                foreach (var replace in replaces)
                {
                    if (string.Compare(source, i, replace.Key, 0, replace.Key.Length) == 0)
                    {
                        i += replace.Key.Length - 1;
                        sb.Append(replace.Value);
                        anyFound = true;
                        break;
                    }
                }
                if (!anyFound)
                {
                    sb.Append(source[i]);
                }
            }
            return sb.ToString();
        }

        static string EscapeLF(string s)
        {
            var pairs = new List<KeyValuePair<string, string>>();
            pairs.Add(new KeyValuePair<string, string>("\r", "\\r"));
            pairs.Add(new KeyValuePair<string, string>("\n", "\\n"));
            pairs.Add(new KeyValuePair<string, string>("\\", "\\\\"));
            return Translate(s, pairs);
        }

        static string UnescapeLF(string s)
        {
            var pairs = new List<KeyValuePair<string, string>>();
            pairs.Add(new KeyValuePair<string, string>("\\r", "\r"));
            pairs.Add(new KeyValuePair<string, string>("\\n", "\n"));
            pairs.Add(new KeyValuePair<string, string>("\\\\", "\\"));
            return Translate(s, pairs);
        }

        static string EscapeWSP(string s) // should also wrap WSP inside strings not only LF
        {
            var pairs = new List<KeyValuePair<string, string>>();
            pairs.Add(new KeyValuePair<string, string>("\\r", "\r"));
            pairs.Add(new KeyValuePair<string, string>("\n", "\\n"));
            pairs.Add(new KeyValuePair<string, string>("\\", "\\\\"));
            return Translate(s, pairs);
        }

        static string UnescapeWSP(string s)
        {
            var pairs = new List<KeyValuePair<string, string>>();
            pairs.Add(new KeyValuePair<string, string>("\\r", "\r"));
            pairs.Add(new KeyValuePair<string, string>("\\n", "\n"));
            pairs.Add(new KeyValuePair<string, string>("\\\\", "\\"));
            return Translate(s, pairs);
        }

        public class ValueList : List<string>
        {
            public ValueList() { }
            public ValueList(string s) { Decode(s); }

            public void Decode(string s)
            {
                if (!string.IsNullOrEmpty(s))
                {
                    string[] aTokens = s.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                    foreach (string sToken in aTokens)
                    {
                        Add(sToken);
                    }
                }
            }
        }

        public class KeyValueList : Dictionary<string, string>
        {
            public KeyValueList() { }
            public KeyValueList(string s) { Decode(s); }

            public string GetString(string sKey)
            {
                try
                {
                    return UnescapeLF(base[sKey]);
                }
                catch (Exception)
                {
                    return "";
                }
            }

            public int GetInt(string sKey)
            {
                string sValue = GetString(sKey);
                if (!string.IsNullOrEmpty(sValue))
                {
                    try
                    {
                        return Convert.ToInt32(sValue);
                    }
                    catch (Exception) { }
                }
                return 0;
            }

            public long GetLong(string sKey)
            {
                string sValue = GetString(sKey);
                if (!string.IsNullOrEmpty(sValue))
                {
                    try
                    {
                        return Convert.ToInt64(sValue);
                    }
                    catch (Exception) { }
                }
                return 0;
            }

            public double GetDouble(string sKey)
            {
                string sValue = GetString(sKey);
                if (!string.IsNullOrEmpty(sValue))
                {
                    try
                    {
                        return Convert.ToDouble(sValue, CultureInfo.InvariantCulture);
                    }
                    catch (Exception) { }
                }
                return 0.0;
            }

            public void SetValueList(string sKey, List<string> vlList)
            {
                string s = "";
                foreach (string sToken in vlList)
                {
                    if (!string.IsNullOrEmpty(s)) { s += " "; }
                    s += sToken;
                }
                Set(sKey, s);
            }

            public ValueList GetValueList(string sKey)
            {
                var vl = new ValueList();
                vl.Decode(GetString(sKey));
                return vl;
            }

            public KeyValueList GetKeyValueList(string sKey)
            {
                var result = new KeyValueList();
                string sValue = GetString(sKey);
                if (!string.IsNullOrEmpty(sValue))
                {
                    result.Decode(sValue);
                }
                return result;
            }

            public void Set(string sKey, string sValue)
            {
                base[sKey] = EscapeLF(sValue);
            }

            public void SetRaw(string sKey, string sValue)
            {
                base[sKey] = sValue;
            }

            public void Set(string sKey, int nValue)
            {
                SetRaw(sKey, nValue.ToString());
            }

            public void Set(string sKey, long nValue)
            {
                SetRaw(sKey, nValue.ToString());
            }

            public void Set(string sKey, double fValue)
            {
                SetRaw(sKey, String.Format(CultureInfo.InvariantCulture, "{0}", fValue));
            }

            public void SetKeyValueList(string sKey, KeyValueList kvList)
            {
                string s = "";
                foreach (var pair in kvList)
                {
                    s += pair.Key + "=" + pair.Value + "\n";
                    //s += pair.Key + "=" + EncodeLF(pair.Value) + "\n";
                }
                Set(sKey, s);
            }

            //public void SetArray(string sName, List<Message> kvList)
            //{
            //  for (int nCnt = 0; nCnt < kvList.Count; nCnt++) {
            //    string sKey = nCnt + ":" + sName;
            //    string sValue = kvList[nCnt].Encode();
            //    base[sKey] = sValue;
            //  }
            //}

            public void Decode(string sBody)
            {
                string[] aLines = sBody.Split(new char[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
                foreach (string sLine in aLines)
                {
                    int nIndex = sLine.IndexOf("=");
                    if (nIndex > 0)
                    {
                        string sKey = sLine.Substring(0, nIndex);
                        string sValue = sLine.Substring(nIndex + 1);
                        base[sKey] = sValue;
                    }
                }
            }

            public string Encode()
            {
                string sBody = "";
                foreach (var pair in this)
                {
                    if (pair.Key.EndsWith("/Binary"))
                    {
                        sBody += pair.Key + "=" + pair.Value.Length + "\n" + pair.Value;
                    }
                    else
                    {
                        sBody += pair.Key + "=" + pair.Value + "\n";
                    }
                }
                return sBody;
            }

            public override string ToString()
            {
                return Encode();
            }
        }

        public class Message : KeyValueList
        {
            public Message() { }
            public Message(string sData) { Decode(sData); }
        }

        public class Request : Message
        {
        }

        public class Response : Message
        {
            public object Body { get; set; }
            public string Mimetype { get; set; }

            public void HistoryBack()
            {
                Body = "<html><head><script>history.back();</script></head><body>back</body></html>";
                Mimetype = "text/html";
            }
        }

        public class Key
        {
            public const string Method = "Method";
            public const string Status = "Status";
            public const string Message = "Message";
            public const string Format = "Format";
        }

        public class MethodDescription
        {
            public string Description { get; set; }
            public Srpc.Request Request { get; set; }
            public Srpc.Response Response { get; set; }
        }

        public class ServiceDescription : Dictionary<string, MethodDescription>
        {
            override public string ToString()
            {
                string sBody = "";

                foreach (var pair in this)
                {
                    sBody += pair.Key + ": " + pair.Value.Description + "\n";
                    if (pair.Value.Request != null)
                    {
                        sBody += "  In:\n";
                        foreach (var pRequest in pair.Value.Request)
                        {
                            sBody += "    " + pRequest.Key + ": " + pRequest.Value + "\n";
                        }
                    }
                    if (pair.Value.Response != null)
                    {
                        sBody += "  Out:\n";
                        foreach (var pRequest in pair.Value.Response)
                        {
                            sBody += "    " + pRequest.Key + ": " + pRequest.Value + "\n";
                        }
                    }
                    sBody += "\n";
                }

                return sBody;
            }

            public string ToHtml()
            {
                string sBody = "";

                //foreach (var pair in this) {
                //  sBody += pair.Key + ": " + pair.Value.Description + "\n";
                //  if (pair.Value.Request != null) {
                //    sBody += "  In:\n";
                //    foreach (var pRequest in pair.Value.Request) {
                //      sBody += "    " + pRequest.Key + ": " + pRequest.Value + "\n";
                //    }
                //  }
                //  if (pair.Value.Response != null) {
                //    sBody += "  Out:\n";
                //    foreach (var pRequest in pair.Value.Response) {
                //      sBody += "    " + pRequest.Key + ": " + pRequest.Value + "\n";
                //    }
                //  }
                //  sBody += "\n";
                //}

                return sBody;
            }
        }

    }
}
