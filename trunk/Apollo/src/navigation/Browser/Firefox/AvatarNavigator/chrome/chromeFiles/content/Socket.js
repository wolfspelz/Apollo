
var Socket = function()
{
  this.bOpen = false;
  this.bConnected = false;
  this.nCheckAliveCnt = 0;
  this.checkAliveTimer = null;
  
  this.transportService = Components.classes["@mozilla.org/network/socket-transport-service;1"].getService(Components.interfaces.nsISocketTransportService);
  if (!this.transportService) { throw ("Socket: getService(nsISocketTransportService) failed"); }
  
  this.transport = null;
  this.outStream = null;
  this.asciiInStream = null;
  this.inPump = null;
  this.observer = null;
}

Socket.kConnectionRefused = 2152398861;

// The observer gets events for connect, disconnect and received data
// 
// var observer = {
//   onConnected: function () { },
//   onDisconnected: function () { },
//   onReceiveData:   function (data) {}
// }
Socket.prototype.connect = function(sHostname, nPort, observer)
{  
  anLogVerbose('Socket.connect ' + sHostname + ':' + nPort);

  this.observer = observer;

  this.transport = this.transportService.createTransport(null, 0, sHostname, nPort, null);
  if (!this.transport) { throw ("Socket.connect: createTransport() failed"); }
  
  this.outStream = this.transport.openOutputStream(0,0,0);
  if (!this.outStream) { throw ("Socket.connect: transport.openOutputStream() failed"); }
  
  var inStream = this.transport.openInputStream(0,0,0);
  if (!inStream) { throw ("Socket.connect: transport.openInputStream() failed"); }
  
  this.asciiInStream = Components.classes["@mozilla.org/scriptableinputstream;1"].createInstance(Components.interfaces.nsIScriptableInputStream);
  if (!this.asciiInStream) { throw ("Socket.connect: createInstance(nsIScriptableInputStream) failed"); }

  this.asciiInStream.init(inStream);
  
  this.inPump = Components.classes["@mozilla.org/network/input-stream-pump;1"].createInstance(Components.interfaces.nsIInputStreamPump);
  if (!this.inPump) { throw ("Socket.connect: createInstance(nsIInputStreamPump) failed"); }
  
  this.inPump.init(inStream, -1, -1, 0, 0, false);
  
  var streamListener = {
    socket: this,
    onStartRequest: function(request, context)
    {
      //anLogTrace('Socket.onStartRequest');
    },
    onStopRequest: function(request, context, status)
    {
      //anLogTrace('Socket.onStopRequest status=' + status);
      this.socket.stopCheckAliveTimer();
      this.socket.observer.onDisconnected();
      this.socket.bConnected = false;
      
      if (this.socket.inPump) {
        this.socket.inPump.cancel(null);
      }

      if (this.socket.asciiInStream) {
        this.socket.asciiInStream.close(status);
      }

      if (this.socket.outStream) {
        this.socket.outStream.close(null);
      }

      if (this.socket.transport) {
        this.socket.transport.close(null);
        this.socket.transport = null;
      }
    },
    onDataAvailable: function(request, context, inputStream, offset, count)
    {
      try {
        var sData = this.socket.asciiInStream.read(count);
        //anLogTrace('Socket.onDataAvailable sData="' + sData + '"');
        this.socket.observer.onReceiveData(sData);
      } catch (ex) {
        anLogError('Socket.onDataAvailable Exception: ' + ex);
      }
    },
  }
  
  this.inPump.asyncRead(streamListener, null);
  //anLogTrace('Socket.connect inPump.asyncRead');
  
  this.bOpen = true;
  this.nCheckAliveCnt = 0;
  this.startCheckAliveTimer();
}

// -------------------------------

// The weird checkAliveTimer thing generates a reliable async onConnect 
// notification because streamListener.onStartRequest does not fire on 
// connect. Rather it comes on the first data and just before disconnect,
// which is quite stupid. Yes, onStopRequest and onStartRequest are 
// balanced. Thanks a lot. Still, sending a onStartRequest/onStopRequest 
// pair when connect fails is a strange idea. We want to know when 
// the connect succeeds, so that we can send a HELO. Therefore we have 
// to care for ourself, as always, sigh.

Socket.prototype.startCheckAliveTimer = function()
{
  this.stopCheckAliveTimer();
  this.checkAliveTimer = setTimeout(this.onCheckAlive.bind(this), 200);  
}

Socket.prototype.stopCheckAliveTimer = function()
{
  if (this.checkAliveTimer) {
    clearTimeout(this.checkAliveTimer);
    this.checkAliveTimer = null;
  }
}

Socket.prototype.onCheckAlive = function()
{

  this.checkAliveTimer = null;
  this.nCheckAliveCnt++;
  anLogTrace('Socket.onCheckAlive ' + this.nCheckAliveCnt);
  
  if (this.bOpen && this.transport && this.transport.isAlive()) {
    var bWasConnected = this.bConnected;
    if (!bWasConnected) {
      this.bConnected = true;
      this.observer.onConnected();
    }
  } else {
    if (this.nCheckAliveCnt < 100) {
      this.startCheckAliveTimer();
    } else {
      this.disconnect();
    }
  }
}

// -------------------------------

Socket.prototype.send = function(sData)
{
  if (this.bConnected) {
    anLogTrace('Socket.send: ' + sData);
    this.outStream.write(sData, sData.length);
  } else {
    anLogTrace('Socket.send not connected');
  }
}

Socket.prototype.disconnect = function()
{
  anLogTrace('Socket.disconnect');

  this.bOpen = false;
  this.bConnected = false;

  if (this.transport) {
    this.transport.close(null);
    this.transport = null;
  }
}

