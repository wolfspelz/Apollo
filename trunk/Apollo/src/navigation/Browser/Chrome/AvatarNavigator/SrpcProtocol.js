
// The host does the I/O. The protcol is rather passive.
// On the receiving side it parses data and generates messages.
// On the sender side it manages request completion. 
// 
// There is a kind of symmetry:
// Receiving side: 
//   host sends data to the protocol, protocol parses the data and
//   sends complete messages to the host
// Sending side:
//   host sends complete messages to the protocol. Protocol 
//   processes the message and sends data to the host
// 
// overwrite:
// mySrpcProtocol.onReceiveMessage = function(msg) { };
// mySrpcProtocol.onSendData = function(sData) { };

var SrpcProtocol = function()
{
  this.requests = new Object();
  this.nSrpcId = 1;
}

SrpcProtocol.prototype.handleMessage= function(sData)
{
  var msg = new SrpcMessage();
  msg.fromString(sData);

  var sStatus = msg.getString('Status');
  if (sStatus != '') {
    var sSrpcId = msg.getString('SrpcId');
    var req = this.requests[sSrpcId];
    delete this.requests[sSrpcId];
    if (req != undefined) {
      if (req.fCallback != undefined) {
        req.fCallback(msg);
      }
    }
  } else {
    this.onReceiveMessage(msg);
  }
}

SrpcProtocol.prototype.sendRequest = function (msg, fCallback)
{
  anLogTrace('SrpcProtocol.sendRequest');

  if (fCallback != null) {
    var sSrpcId = '_' + this.nSrpcId++;
    msg.setString('SrpcId', sSrpcId);

    this.requests[sSrpcId] = { 'msg': msg, 'fCallback': fCallback };
  }

  var sMsg = msg.toString();
  this.onSendData(sMsg + '\n');
}

