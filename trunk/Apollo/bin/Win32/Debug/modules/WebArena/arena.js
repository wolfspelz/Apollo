
function Arena(sDomId)
{
  this.sDomId = sDomId;
}

Arena.prototype = {

  Startup: function()
  {
    //$('#' + this.sDomId).html('<b>Arena</b>');
    $('#' + this.sDomId).append('<div id="iMeta"><p id="iDocumentUrl"></p><p id="iLocationUrl"></p><p id="iLocationState"></p></div>');
  },

  AddAvatar: function (sId, sNickname, sImageUrl, nX)
  {
    $('#' + this.sDomId).append(
  '<div id="iParticipant_' + sId.toDomCompatible() + '" class="cParticipant" style="display:none">'
+ '  <div class="cChatWrapper">'
+ '    <div class="cChat"></div>'
+ '  </div>'
+ '  <div class="cImageWrapper">'
+ '    <div class="cAvatar"></div>'
+ '    <div class="cNickname"></div>'
+ '  </div>'
+ '</div>'
    );

    this.SetAvatarPosition(sId, nX);
    this.SetAvatarNickname(sId, sNickname);
    this.SetAvatarImage(sId, sImageUrl);
    this.ShowAvatar(sId);
  },

  RemoveAvatar: function (sId)
  {
    $('#iParticipant_' + sId.toDomCompatible()).remove();
  },

  ShowAvatar: function (sId)
  {
    $('#iParticipant_' + sId.toDomCompatible()).css('display', '');
  },

  HideAvatar: function (sId)
  {
    $('#iParticipant_' + sId.toDomCompatible()).css('display', 'none');
  },

  SetAvatarPosition: function (sId, nX)
  {
    $('#iParticipant_' + sId.toDomCompatible()).css('left', nX + 'px');
  },

  SetAvatarNickname: function (sId, sNickname)
  {
    $('#iParticipant_' + sId.toDomCompatible() + ' .cNickname').html(sNickname);
  },

  SetAvatarImage: function (sId, sUrl)
  {
    $('#iParticipant_' + sId.toDomCompatible() + ' .cAvatar').css('background-image', 'url(' + sUrl + ')');
  },

  SetAvatarChat: function (sId, sText)
  {
    $('#iParticipant_' + sId.toDomCompatible() + ' .cChat').html(sText);
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
    $('#iLocationState').removeClass().addClass("sState");
  },

  dummy: 1
}
