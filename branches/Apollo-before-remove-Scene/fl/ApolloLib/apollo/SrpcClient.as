package apollo
{
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.events.TimerEvent;
	import flash.net.Socket;
	import flash.utils.Timer;
	
	public class SrpcClient
	{
		public var fRecvMessageHandler_:Function = null;
		public var fConnectionEstablishedHandler_:Function = null;
	  	public var fConnectionTerminatedHandler_:Function = null;
	 	public var fConsoleHandler_:Function = null;
	 		
		protected var s_:Socket = null;
		protected var sHost_:String = "localhost";
		protected var nPort_:int = 12345;
		protected var bActive_:Boolean = false;
		protected var bConnected_:Boolean = false;
		protected var bAutoReconnect_:Boolean = true;
		protected var nReconnectDelay_:int = 0;
		protected var nMinReconnectDelay_:int = 100;
		protected var nMaxReconnectDelay_:int = 5000;
		protected var reconnectTimer_:Timer = null;
		protected var sCurrentData_:String = "";

		public function SrpcClient():void
		{
			this.s_ = new Socket();
			this.s_.addEventListener(Event.CONNECT, onConnected);
			this.s_.addEventListener(ProgressEvent.SOCKET_DATA, onDataIn);
			this.s_.addEventListener(Event.CLOSE, onDisconnected);
			this.s_.addEventListener(IOErrorEvent.IO_ERROR, onError);
		}

		// ---------------------------------------------------------
		// Events

		protected function onConnected(e:Event):void
		{
			_trace("SrpcClient.onConnected");
			this.bConnected_ = true;
			this.nReconnectDelay_ = this.nMinReconnectDelay_;

			if (this.fConnectionEstablishedHandler_ != null) {
				this.fConnectionEstablishedHandler_();
			}
		}

		protected function onDataIn(e:ProgressEvent):void
		{
			var sData:String = s_.readUTFBytes(s_.bytesAvailable);			
			this.sCurrentData_ += sData;
			
			var bDone:Boolean = false;
			while (!bDone) {
				var nMsgSep:int = this.sCurrentData_.indexOf("\n\n");
				if (nMsgSep == 0) {
					this.sCurrentData_ = this.sCurrentData_.substr(2);
				} else if (nMsgSep > 0) {
					var sMsg:String = this.sCurrentData_.substr(0, nMsgSep);
					this.sCurrentData_ = this.sCurrentData_.substr(nMsgSep + 2);
					var msg:SrpcMessage = new SrpcMessage();
					msg.fromString(sMsg);
					if (fRecvMessageHandler_ != null ) {
						this.fRecvMessageHandler_(msg);
					}
				} else {
					bDone = true;
				}
			} // while (!bFinished)
			
		}

		protected function onDisconnected(e:Event):void
		{
			_trace("SrpcClient.onDisconnected");
			this.bConnected_ = false;
			this.bActive_ = false;
			this.cleanup();

			if (this.fConnectionTerminatedHandler_ != null) {
				this.fConnectionTerminatedHandler_();
			}
		
			if (this.bAutoReconnect_) {
				this.reconnect();
			}
		}

		protected function onError(e:IOErrorEvent):void
		{
			_trace("SrpcClient.onError");
			var bWasConnected:Boolean = this.bConnected_;
			this.disconnect();			
			this.cleanup();

			if (bWasConnected) {
				if (this.fConnectionTerminatedHandler_ != null) {
					this.fConnectionTerminatedHandler_();
				}
			}

			if (this.bAutoReconnect_) {
				this.reconnect();
			}
		}

		// ---------------------------------------------------------
		// Internal

		protected function _trace(sText:String):void
		{
			if (fConsoleHandler_ != null ) {
				this.fConsoleHandler_(sText);
			}
		}


		// ---------------------------------------------------------
		// Low level publics

		public function connect(sHost:String, nPort:int):void
		{
			this.sHost_ = sHost;
			this.nPort_ = nPort;

			if (!this.bConnected_) {
				_trace("SrpcClient.connect " + sHost + " " + nPort);
				this.s_.connect(sHost_, nPort_);
				this.bActive_ = true;
				this.bAutoReconnect_ = true;
			}
		}

		public function disconnect():void
		{
			if (this.bConnected_) {
				_trace("SrpcClient.disconnect");
				this.s_.close();
				this.bConnected_ = false;
				this.bActive_ = false;
				this.bAutoReconnect_ = false;
				this.cleanup();

				if (this.fConnectionTerminatedHandler_ != null) {
					this.fConnectionTerminatedHandler_();
				}
		  }
		}
		
		public function send(sText:String):void
		{						
			if (this.bConnected_) {
				var sFixed:String = "";
				for (var i:int = 0; i < sText.length; i++) {
					if (sText.charCodeAt(i) == 13) {
						sFixed += "\n";
					} else {
						sFixed += sText.charAt(i);
					}
				}
				
				this.s_.writeUTFBytes(sFixed);
				this.s_.flush();
			}
		}

		// ---------------------------------------------------------
		// Reconnect management

		public function cleanup():void
		{
			this.sCurrentData_ = "";
			//this.currentMsg_ = null;
		}

		public function reconnect():void
		{
			this.nextReconnectDelay();
			this.reconnectTimer_ = new Timer(this.nReconnectDelay_, 1);
			this.reconnectTimer_.addEventListener(TimerEvent.TIMER_COMPLETE, onReconnectTimerComplete);
			this.reconnectTimer_.start();
		}

		protected function onReconnectTimerComplete(event:TimerEvent):void
		{
			this.connect(this.sHost_, this.nPort_);
			this.bActive_ = false;
		}

		protected function nextReconnectDelay():void
		{
			var nOld:int = this.nReconnectDelay_;
			if (this.nReconnectDelay_ < this.nMinReconnectDelay_) {
				this.nReconnectDelay_ = this.nMinReconnectDelay_;
			} else {
				this.nReconnectDelay_ = this.nReconnectDelay_ * 2;
				if (this.nReconnectDelay_ > this.nMaxReconnectDelay_) {
					this.nReconnectDelay_ = this.nMaxReconnectDelay_;
				}
			}
			_trace("SrpcClient.nextReconnectDelay " + nOld + " -> " + this.nReconnectDelay_);
		}

		// ---------------------------------------------------------
		// High level interface

		public function sendMessage(msg:SrpcMessage):void
		{
			this.send(msg.toString() + "\n");
		}

		// ---------------------------------------------------------
		// Unittest

		protected function testParseMessage1(sName:String, sMsg:String, aExpected:Array):String
		{
			var s:String = "";

			var msg:SrpcMessage = new SrpcMessage();
			msg.fromString(sMsg);
			for (var i:String in aExpected) { 
				if (aExpected[i] == msg.List[i]) {} else { 
					s = sName + ": Expectation is missing " + i + "=" + aExpected[i] + " in parsed message";
				}
			}
			for (var j:String in msg.List) {
				if (msg.List[j] == aExpected[j]) {} else {
					s = sName + ": Parsed message is missing " + j + "=" + msg.List[j] + " in expectation";
				}
			}

			return s;
		}

		protected function testParseMessage():String
		{
			var s:String = "";
			var aExpected:Array = null;
			
			if (!s) {
				aExpected = new Array();
				aExpected["Method"] = "Test";
				s = this.testParseMessage1("0a", "Method=Test", aExpected);
			}
			
			if (!s) {
				aExpected = new Array();
				aExpected["Method"] = "Test";
				s = this.testParseMessage1("0b", "Method=Test\n", aExpected);
			}
			
			if (!s) {
				aExpected = new Array();
				aExpected["Method"] = "Test";
				aExpected["Key1"] = "Value1";
				aExpected["Key2"] = "Value2";
				s = this.testParseMessage1("1", "Method=Test\nKey1=Value1\nKey2=Value2", aExpected);
			}
			
			if (!s) {
				aExpected = new Array();
				aExpected["Method"] = "Test";
				aExpected["Key1"] = "Value1";
				aExpected["Key2"] = "";
				s = this.testParseMessage1("2a", "Method=Test\nKey1=Value1\nKey2=", aExpected);
			}
			
			if (!s) {
				aExpected = new Array();
				aExpected["Method"] = "Test";
				aExpected["Key1"] = "Value1";
				s = this.testParseMessage1("2b", "Method=Test\nKey1=Value1\nKey2", aExpected);
			}
			
			if (!s) {
				aExpected = new Array();
				aExpected["Method"] = "Test";
				aExpected["Key1"] = "";
				aExpected["Key2"] = "Value2";
				s = this.testParseMessage1("3a", "Method=Test\nKey1=\nKey2=Value2", aExpected);
			}
			
			if (!s) {
				aExpected = new Array();
				aExpected["Method"] = "Test";
				aExpected["Key2"] = "Value2";
				s = this.testParseMessage1("3b", "Method=Test\nKey1\nKey2=Value2", aExpected);
			}
			
			if (!s) {
				aExpected = new Array();
				aExpected["Method"] = "Test";
				aExpected["Key"] = "a=b\\nc=d";
				s = this.testParseMessage1("4a", "Method=Test\nKey1\nKey=a=b\\nc=d", aExpected);
			}
			
			if (!s) { return s; } else { return "SrpcClient.testParseMessage: " + s; }
		}

		protected function testEncodeCString1(sName:String, sValue:String, sExpected:String):String
		{
			var s:String = "";
			var sEncoded:String = SrpcMessage.encodeCString(sValue);
			if (sEncoded != sExpected) {
				s = sName + ": Got=" + sEncoded + " expected=" + sExpected;
			}
			return s;
		}

		protected function testDecodeCString1(sName:String, sValue:String, sExpected:String):String
		{
			var s:String = "";
			var sDecoded:String = SrpcMessage.decodeCString(sValue);
			if (sDecoded != sExpected) {
				s = sName + ": Got=" + sDecoded + " expected=" + sExpected;
			}
			return s;
		}

		protected function testEncodeCString():String
		{
			var s:String = "";
			
			if (!s) { s = this.testEncodeCString1("101", "Test", "Test"); }
			if (!s) { s = this.testEncodeCString1("102", "Test\nx", "Test\\nx"); }
			if (!s) { s = this.testEncodeCString1("103", "Test\\nx", "Test\\\\nx"); }
			if (!s) { s = this.testEncodeCString1("104", "Test\nx\r\ny", "Test\\nx\\r\\ny"); }
			
			if (!s) { s = this.testDecodeCString1("201", "Test", "Test"); }
			if (!s) { s = this.testDecodeCString1("202", "Test\\nx", "Test\nx"); }
			if (!s) { s = this.testDecodeCString1("203", "Test\\\\nx", "Test\\nx"); }
			if (!s) { s = this.testDecodeCString1("204", "Test\\nx\\r\\ny", "Test\nx\r\ny"); }
			
			if (!s) { return s; } else { return "SrpcClient.testEncodeCString: " + s; }
		}

		public function unitTest():String
		{
			var s:String = "";
			
			if (!s) { s = this.testParseMessage(); }
			if (!s) { s = this.testEncodeCString(); }
			
			return s;
		}

	}
}
