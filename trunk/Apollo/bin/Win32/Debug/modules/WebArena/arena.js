
function Arena(sDomId)
{
  this.sDomId = sDomId;
}

Arena.prototype = {

  Startup: function()
  {
    $('#' + this.sDomId).html('<b>Arena</b>');
    $('#' + this.sDomId).append('<div id="iMeta"><p id="iDocumentUrl"></p><p id="iLocationUrl"></p><p id="iLocationState"></p></div>');
  },

  ShowAvatar: function (sId, sNickname, sImageUrl, nX)
  {
//    Log.Debug('sId=' + sId + ' sNickname=' + sNickname + ' sImageUrl=' + sImageUrl + ' nX=' + nX);
//    api.logDebug('sId=' + sId + ' sNickname=' + sNickname + ' sImageUrl=' + sImageUrl + ' nX=' + nX);

    $('#' + this.sDomId).append(
  '<div class="Participant" id="iParticipant_' + sId.toDomCompatible() + '" style="left:' + nX + ';">'
+ '  <img class="Avatar" src="' + sImageUrl + '" />'
+ '  <div class="Nickname">' + sNickname + '</div>'
+ '</div>'
    );
  },

  HideAvatar: function (sId)
  {
    $('#iParticipant_' + sId.toDomCompatible()).remove();
  },

  SetAvatarPosition: function (sId, nX)
  {
    $('#iParticipant_' + sId.toDomCompatible()).css('left', nX);
  },

  SetAvatarNickname: function (sId, sNickname)
  {
    $('#iParticipant_' + sId.toDomCompatible() + ' .Nickname').html(sNickname);
  },

  SetAvatarImage: function (sId, sUrl)
  {
    $('#iParticipant_' + sId.toDomCompatible() + ' .Avatar').attr('src', sUrl);
  },

  SetDocumentUrl: function (sUrl)
  {
    $('#iDocumentUrl').html(sUrl);
  },

  SetLocationUrl: function (sUrl)
  {
    $('#iLocationUrl').html(sUrl);
  },

  SetLocationState: function (sState)
  {
    $('#iLocationState').html(sState);
    //$('#iLocationState').removeClass().addClass("sState");
  },

  dummy: 1
}
