
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
// var host = {
//   onReceiveMessage: function (msg) { },
//   onSendData: function (sData) { },
// }

var SrpcProtocol = function(host)
{
  this.requests = new Object();
  this.nSrpcId = 1;
  this.host = host;
  this.sCurrentData_ = '';
}

SrpcProtocol.prototype.parseData = function(sData)
{
  anLogTrace('SrpcProtocol.parseData: ' + sData);
  this.sCurrentData_ += sData;

  var bDone = false;
  while (!bDone) {
    var nMsgSep = this.sCurrentData_.indexOf("\n\n");
    if (nMsgSep == 0) {
      this.sCurrentData_ = this.sCurrentData_.substr(2);
    } else if (nMsgSep > 0) {
      var sMsg = this.sCurrentData_.substr(0, nMsgSep);
      this.sCurrentData_ = this.sCurrentData_.substr(nMsgSep + 2);
      var msg = new SrpcMessage();
      msg.fromString(sMsg);

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
        this.host.onReceiveMessage(msg);
      }

    } else {
      bDone = true;
    }
  } // while (!bFinished)

}

SrpcProtocol.prototype.sendRequest = function(msg, fCallback)
{
  anLogTrace('SrpcProtocol.sendRequest');

  var sSrpcId = '_' + this.nSrpcId++;
  msg.setString('SrpcId', sSrpcId);

  this.requests[sSrpcId] = {'msg': msg, 'fCallback': fCallback};

  var sMsg = msg.toString();
  this.host.onSendData(sMsg + '\n');
}

