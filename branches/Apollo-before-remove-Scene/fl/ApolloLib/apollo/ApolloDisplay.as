package apollo
{
	import flash.desktop.NativeApplication;
	import flash.display.NativeWindow;
	import flash.events.InvokeEvent;

	public class ApolloDisplay
	{
		public static const MethodPrefix_Local:String = "Local.";
		public static const Method_Console:String = "Local.Console";
		public static const Method_Connected:String = "Local.Connected";
		public static const Method_Disconnected:String = "Local.Disconnected";
		public static const Method_Shutdown:String = "Local.Shutdown";
		public static const Method_CommandlineBegin:String = "Local.CommandlineBegin";
		public static const Method_CommandlineParam:String = "Local.CommandlineParam";
		public static const Method_CommandlineEnd:String = "Local.CommandlineEnd";
		
		public static const MethodPrefix_Host:String = "Host.";
		public static const Method_Hello:String = "Host.Hello";
		public static const Method_Log:String = "Host.Log";
		public static const Method_GetConfig:String = "Host.GetConfig";
		public static const Method_SetConfig:String = "Host.SetConfig";
		
		public static const MethodPrefix_Display:String = "Display.";
		public static const Method_SetTitle:String = "Display.SetTitle";
		public static const Method_SetVisibility:String = "Display.SetVisibility";
		public static const Method_SetPosition:String = "Display.SetPosition";
		public static const Method_Loaded:String = "Display.Loaded";
		public static const Method_BeforeUnload:String = "Display.BeforeUnload";
		public static const Method_Unloaded:String = "Display.Unloaded";
		
		public static const LogLevel_Verbose:int = 6;
		public static const LogLevel_Info:int = 5;
		public static const LogLevel_Debug:int = 4;
		public static const LogLevel_Warning:int = 3;
		public static const LogLevel_Error:int = 2;
		public static const LogLevel_Fatal:int = 1;

		protected static var instance_:ApolloDisplay = null;

		protected var fSrpcMessageHandler_:Function = null;
		protected var client_:SrpcClient = null;
		protected var bConnected_:Boolean = true;
		protected var sHost_:String = "192.168.1.38";
		protected var nPort_:int = 12345;
		protected var sId_:String = "0";
		protected var nRequestId_:int = 0;
		protected var callbacks_:Array = new Array();
		protected var window_:NativeWindow = null;

		public function ApolloDisplay():void
		{
			flash.desktop.NativeApplication.nativeApplication.addEventListener(flash.events.InvokeEvent.INVOKE, onInvoke);
			this.client_ = new SrpcClient();
			this.client_.fRecvMessageHandler_ = onRecvMessage;
			this.client_.fConnectionEstablishedHandler_ = onConnected;
			this.client_.fConnectionTerminatedHandler_ = onDisconnected;
			this.client_.fConsoleHandler_ = onConsole;
		}

		// Singleton as getter
		public static function get instance():ApolloDisplay
		{
			if (ApolloDisplay.instance_ == null) {
				ApolloDisplay.instance_ = new ApolloDisplay();
			}
			return ApolloDisplay.instance_;
		}

		// ---------------------------------------------------------

		public function setMessageHandler(fSrpcMessageHandler:Function):void
		{
			this.fSrpcMessageHandler_ = fSrpcMessageHandler;
		}
		
		public function setNativeWindow(window:NativeWindow):void
		{
			this.window_ = window;
		}

		public function start():void
		{
			if (this.client_) {
				this.client_.connect(this.sHost_, this.nPort_);
			}
		}

		//public function testCallback(msg:SrpcMessage, ref):void
		//{
		//	logInfo("ApolloDisplay.testCallback", msg.getString("SrpcId"));
		//}

		public function send(msg:SrpcMessage):void
		{
			if (this.client_) {
				//this.request(msg, testCallback);
				this.client_.send(msg.toString() + "\n");
			}
		}

		public function request(msg:SrpcMessage, fCallback:Function, ref:* = null):void
		{
			if (fCallback != null) {
				var sRequestId:String = ApolloDisplay.instance.getNextRequestId();
				this.callbacks_[sRequestId] = [fCallback, ref];
				msg.setString(SrpcMessage.Param_RequestId, sRequestId);
				ApolloDisplay.instance.client_.send(msg.toString() + "\n");
			
			} else {
				if (this.client_) {
					this.send(msg);
				}
			}
		}

		public function setConfig(sModule:String, sKey:String, sValue:String):void
		{
			var msg:SrpcMessage = new SrpcMessage();
			msg.setString("Method", ApolloDisplay.Method_SetConfig);
			msg.setString("sModule", sModule);
			msg.setString("sKey", sKey);
			msg.setString("sValue", sValue);
			this.request(msg, configCallback)
		}

		public function requestConfig(sModule:String, sKey:String, fCallback:Function, ref:* = null):void
		{
			var msg:SrpcMessage = new SrpcMessage();
			msg.setString("Method", ApolloDisplay.Method_GetConfig);
			msg.setString("sModule", sModule);
			msg.setString("sKey", sKey);
			this.request(msg, configCallback, [fCallback, ref])
		}

		// ---------------------------------------------------------

		public function configCallback(msg:SrpcMessage, ref:*):void
		{
			var sValue:String = msg.getString("sValue");
			ref[0](sValue, ref[1]);
		}

		public function sendRaw(sData:String):void
		{
			if (this.client_) {
				this.client_.send(sData);
			}
		}

		public function receive(msg:SrpcMessage):void
		{
			if (this.fSrpcMessageHandler_ != null) {
				this.fSrpcMessageHandler_(msg);
			}
		}

		public function getNextRequestId():String
		{
			this.nRequestId_++;
			var d:Date = new Date();
			return String(d.getTime()) + String(d.getMilliseconds()) + String(this.nRequestId_);
		}

		public function logVerbose(sContext:String, sMessage:String):void { this.log(ApolloDisplay.LogLevel_Verbose, sContext, sMessage); }
		public function logInfo(sContext:String, sMessage:String):void { this.log(ApolloDisplay.LogLevel_Info, sContext, sMessage); }
		public function logDebug(sContext:String, sMessage:String):void { this.log(ApolloDisplay.LogLevel_Debug, sContext, sMessage); }
		public function logWarning(sContext:String, sMessage:String):void { this.log(ApolloDisplay.LogLevel_Warning, sContext, sMessage); }
		public function logError(sContext:String, sMessage:String):void { this.log(ApolloDisplay.LogLevel_Error, sContext, sMessage); }
		public function logFatal(sContext:String, sMessage:String):void { this.log(ApolloDisplay.LogLevel_Fatal, sContext, sMessage); }

		// ---------------------------------------------------------

		public function onInvoke(invokeEvent:InvokeEvent):void
		{
			this.parseArgs(invokeEvent.arguments);
			this.logInfo("ApolloDisplay.onInvoke", "Host=" + this.sHost_);
			this.logInfo("ApolloDisplay.onInvoke", "Port=" + this.nPort_);
			this.logInfo("ApolloDisplay.onInvoke", "Id=" + this.sId_);
			this.start();
		}

		// ---------------------------------------------------------

		protected function log(nLevel:int, sContext:String, sMessage:String):void
		{
			var msg:SrpcMessage = new SrpcMessage();
			msg.setString("Method", ApolloDisplay.Method_Log);
			msg.setInt("nLevel", nLevel);
			msg.setString("sContext", sContext);
			msg.setString("sMessage", sMessage);
			this.send(msg);
		}

		protected function onRecvMessage(msg:SrpcMessage):void
		{
			var sMethod:String = msg.getString("Method");
			if (sMethod != "") {
				switch (sMethod) {
					case ApolloDisplay.Method_Hello:
						break;
					
					case ApolloDisplay.Method_SetTitle:
						this.window_.title = msg.getString("sTitle");
						break;
			
					case ApolloDisplay.Method_SetPosition:
						if (msg.getString("nLeft") != "") {
							this.window_.x = msg.getInt("nLeft");
						}
						if (msg.getString("nTop") != "") {
							this.window_.y = msg.getInt("nTop");
						}
						if (msg.getString("nWidth") != "") {
							this.window_.width = msg.getInt("nWidth");
						}
						if (msg.getString("nHeight") != "") {
							this.window_.height = msg.getInt("nHeight");
						}
						break;
			
					case ApolloDisplay.Method_SetVisibility:
						
						if (msg.getInt("nVisible") > 0) {
							//this.window_.visible = true;
							this.window_.activate();
						} else {
							this.window_.visible = false;
						}
						
						break;
						
						
					default:
						this.receive(msg);
				} // switch
				
			} else {

				// Is it a response?
				var sStatus:String = msg.getString("Status");
				if (sStatus != "") {
					var sSrpcId:String = msg.getString("SrpcId");
					if (sSrpcId != "") {
						var callback:Array = this.callbacks_[sSrpcId];
						if (!callback) {
							this.receive(msg);
							
							//logError("ApolloDisplay.onRecvMessage", "No callback for SrpcId=" + msg.getString("SrpcId"));
						} else {
							callback[0](msg, callback[1]);
							delete this.callbacks_[sSrpcId];
						}
					}
				}
				
			} // !sMethod
		}
		
		protected function onConnected():void
		{
			this.bConnected_ = true;
			
			var msg1:SrpcMessage = new SrpcMessage();
			msg1.setString("Method", ApolloDisplay.Method_Hello);
			msg1.setString("sId", this.sId_);
			this.send(msg1);

			var msg2:SrpcMessage = new SrpcMessage();
			msg2.setString("Method", ApolloDisplay.Method_Connected);
			this.receive(msg2);
		}
		
		protected function onDisconnected():void
		{
			this.bConnected_ = false;
			var msg:SrpcMessage = new SrpcMessage();
			msg.setString("Method", ApolloDisplay.Method_Disconnected);
			this.receive(msg);
		}
		
		protected function onConsole(sText:String):void
		{
			var msg:SrpcMessage = new SrpcMessage();
			msg.setString("Method", ApolloDisplay.Method_Console);
			msg.setString("sText", sText);
			this.receive(msg);
		}

		protected function parseArgs(aArgs:Array):void
		{
			var msg:SrpcMessage = null;
			
			msg = new SrpcMessage();
			msg.setString("Method", ApolloDisplay.Method_CommandlineBegin);
			this.receive(msg);

			var sName:String = "";
			for (var i:String in aArgs) {
				var sToken:String = aArgs[i];

				msg = new SrpcMessage();
				msg.setString("Method", ApolloDisplay.Method_CommandlineParam);
				msg.setString("sToken", sToken);
				this.receive(msg);

				if (sName == "") {
					switch (aArgs[i]) {
						case "-host" :
							sName = sToken;
							break;
						case "-port" :
							sName = sToken;
							break;
						case "-id" :
							sName = sToken;
							break;
						default :
							sName = "";
					}
				} else {
					switch (sName) {
						case "-host" :
							this.sHost_ = sToken;
							break;
						case "-port" :
							this.nPort_ = parseInt(sToken);
							break;
						case "-id" :
							this.sId_ = sToken;
							break;
					}

					sName = "";
				}
			}

			msg = new SrpcMessage();
			msg.setString("Method", ApolloDisplay.Method_CommandlineEnd);
			this.receive(msg);
		}

		// ---------------------------------------------------------

		protected function testParseArgs():String
		{
			var s:String = "";

			var d:ApolloDisplay = new ApolloDisplay();
			var a:Array = new Array("-port", "4145", "-host", "localhost", "-id", "123");
			d.parseArgs(a);
			
			if (!s) { if (d.sHost_ != "localhost") { s = "sHost_ wrong"; }}
			if (!s) { if (d.nPort_ != 4145) { s = "nPort_ wrong"; }}
			if (!s) { if (d.sId_ != "123") { s = "sId_ wrong"; }}

			if (!s) { return s; } else { return "ApolloDisplay.testParseArgs: " + s; }
		}

		public function unitTest():String
		{
			var s:String = "";

			if (!s) { s = this.testParseArgs(); }
			if (!s) { if (this.client_ != null) { s = this.client_.unitTest(); }}

			return s;
		}

		// ---------------------------------------------------------
	}
}