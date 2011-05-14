
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

  AddAvatar: function (hParticipant, sNickname, sImageUrl, nX)
  {
    $('#' + this.sDomId).append(''
//  + '<div id="iParticipant_' + hParticipant.toDomCompatible() + '" class="cParticipant" style="display:none">'
//  + '  <div class="cChatWrapper">'
//  + '    <div class="cChat"></div>'
//  + '  </div>'
//  + '  <div class="cImageWrapper">'
//  + '    <div class="cAvatar"></div>'
//  + '    <div class="cNickname"></div>'
//  + '  </div>'
//  + '</div>'

    + '<div class="cParticipant" id="iParticipant_' + hParticipant.toDomCompatible() + '" style="display:none;">'
    + '  <div class="cCenter">'
    + '    <div class="cChatWrapper">'
    + '      <div class="cChatContainer">'
    + '        <div class="cChat"></div>'
    + '      </div>'
    + '    </div>'
    + '    <div class="cImageWrapper">'
    + '      <div class="cImage"></div>'
    + '      <div class="cNickname">Long Nickname</div>'
    + '    </div>'
    + '  </div>'
    + '</div>'

    );

    this.SetAvatarPosition(hParticipant, nX);
    this.SetAvatarNickname(hParticipant, sNickname);
    this.SetAvatarImage(hParticipant, sImageUrl);
    this.ShowAvatar(hParticipant);
  },

  RemoveAvatar: function (hParticipant)
  {
    $('#iParticipant_' + hParticipant.toDomCompatible()).remove();
  },

  ShowAvatar: function (hParticipant)
  {
    $('#iParticipant_' + hParticipant.toDomCompatible()).css('display', '');
  },

  HideAvatar: function (hParticipant)
  {
    $('#iParticipant_' + hParticipant.toDomCompatible()).css('display', 'none');
  },

  SetAvatarPosition: function (hParticipant, nX)
  {
    $('#iParticipant_' + hParticipant.toDomCompatible()).css('left', nX + 'px');
  },

  SetAvatarNickname: function (hParticipant, sNickname)
  {
    $('#iParticipant_' + hParticipant.toDomCompatible() + ' .cNickname').html(sNickname);
  },

  SetAvatarImage: function (hParticipant, sUrl)
  {
    var img = new Image()
    img.onload = function() {
      $('#iParticipant_' + hParticipant.toDomCompatible() + ' .cImage').css('background-image', 'url(' + sUrl + ')');
    }
    img.src = sUrl;
  },

  SetAvatarChat: function (hParticipant, hChat, sText)
  {
    $('#iParticipant_' + hParticipant.toDomCompatible() + ' .cChat').html(sText);
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
