package apollo
{	
	public class SrpcMessage
	{
		public static const Param_RequestId:String = "SrpcId";

		protected var aParams_:Array = null;
		
		public function SrpcMessage():void
    {
			this.aParams_ = new Array();
    }

		// ---------------------------------------------------------
		// Setter
		
		public function setString(sKey:String, sValue:String):void
    {
			//if (sValue.indexOf("\r") >= 0 || sValue.indexOf("\n") >= 0 || sValue.indexOf("\t") >= 0) {
				sValue = SrpcMessage.encodeCString(sValue);
				//this.aParams_[sKey + "/Encoding"] = "cstring";
			//}
			this.aParams_[sKey] = sValue;
    }

		public function set(sKey:String, sValue:String):void
    {
			this.aParams_[sKey] = sValue;
    }

		public function setInt(sKey:String, nValue:int):void
    {
			this.aParams_[sKey] = String(nValue);
    }

		public function set Method(sValue:String):void
    {
			this.setString("Method", sValue);
    }

		public function fromString(sMsg:String):void
		{
			var aLines:Array = sMsg.split("\n");
			if (aLines.length > 0) {
				for (var i:String in aLines) {
					var nEquals:int = aLines[i].indexOf("=");
					if (nEquals > 0) {
						var sKey:String = aLines[i].substr(0, nEquals);
						var sValue:String = aLines[i].substr(nEquals + 1);
						if (sKey != "") {
							if (sValue != "") {
								this.set(sKey, sValue);
							} else {
								this.set(sKey, "");
							}
						}
					}
				}
			}
		}

		public static function encodeCString(sValue:String):String
    {
			var sResult:String = "";
			for (var i:int = 0; i < sValue.length; i++) {
				switch (sValue.charAt(i)) {
					case "\\": sResult += "\\\\"; break;
					case "\r": sResult += "\\r"; break;
					case "\n": sResult += "\\n"; break;
					default: sResult += sValue.charAt(i);
				}
			}
			return sResult;
    }

		// ---------------------------------------------------------
		// Getter
		
		public static function decodeCString(sValue:String):String
	    {
			var sResult:String = "";
			var bEscape:Boolean = false;
			for (var i:int = 0; i < sValue.length; i++) {
				var sChar:String = sValue.charAt(i);
				if (!bEscape) {
					if (sChar == "\\") {
						bEscape = true;
					} else {
						sResult += sChar;
					}
				} else {
					switch (sChar) {
						case "\\": sChar = "\\"; break;
						case "r": sChar = "\r"; break;
						case "n": sChar = "\n"; break;
						default:;
					}
					sResult += sChar;
					bEscape = false;
				}
			}
			return sResult;
    }

		public function getString(sKey:String):String
    {
			var sValue:String = "";
			if (this.aParams_[sKey]) {
				sValue = this.aParams_[sKey];
				//var sEncoding:String = this.aParams_[sKey + "/Encoding"];
				//if (sEncoding == "cstring") {
					sValue = SrpcMessage.decodeCString(sValue);
				//}
			}
			return sValue;
    }

		public function getInt(sKey:String):int
    {
			return parseInt(this.getString(sKey));
    }

		public function get Method():String
    {
			return this.getString("Method");
    }

		public function get List():Array
    {
			return this.aParams_;
    }

		public function toString():String
		{
			var sMsg:String = "";
			for (var sKey:String in this.aParams_) {
				sMsg += sKey + "=" + this.aParams_[sKey] + "\n";
			}
			return sMsg;
    }

	}
}
