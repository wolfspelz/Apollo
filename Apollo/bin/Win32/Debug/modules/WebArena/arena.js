
function Arena(sDomId)
{
  this.sDomId = sDomId;
}

Arena.prototype = {

  Startup: function()
  {
    //$('#' + arena.sDomId).html('<b>Arena</b>');
    $('#' + arena.sDomId).append('<div id="iMeta"><p id="iDocumentUrl"></p><p id="iLocationUrl"></p><p id="iLocationState"></p></div>');
  },

  AddAvatar: function (hParticipant, bSelf, nX)
  {
    $('#' + arena.sDomId).append(''

    + '<div class="cParticipant" id="' + arena.GetParticipantDomId(hParticipant) + '" style="display:none;">'
    + '  <div class="cCenter">'
    + '    <div class="cChatWrapper" style="z-index:30;">'
    + '      <div class="cChatContainer"></div>'
    + '    </div>'
    + '    <div class="cImageWrapper" style="z-index:30;">'
    + '      <div class="cImage" style="z-index:40;"></div>'
    + '      <div class="cNickname" style="z-index:50;">Long Nickname</div>'
    
    + (bSelf ? ''
      + '    <div class="cImageSensor" style="z-index:70;">'
      + '      <div class="cMenu" style="display:none; z-index:80;"><img class="cBubble" src="Bubble.png" /></div>'
      + '      <div class="cChatIn" style="display:none; z-index:100;">'
      + '        <table border="0" cellpadding="0" cellspacing="0"><tr>'
      + '          <td><input type="text" class="cText" size="30" /></td>'
      + '          <td><input type="submit" class="cSend cTranslate" value="Send" /></td>'
      + '          <td><img src="CloseButton.png" class="cCloseButton" /></td>'
      + '        </tr></table>'
      + '      </div>'
      + '    </div>'
      :
      ''
      )
    
    + '    </div>'
    + '  </div>'
    + '</div>'

    );

    arena.SetAvatarPosition(hParticipant, nX);
    arena.ShowAvatar(hParticipant);
    
    if (bSelf) {
      arena.ActivateSelfAvatarElements(hParticipant);
    }
  },

  RemoveAvatar: function (hParticipant)
  {
    $('#' + arena.GetParticipantDomId(hParticipant)).remove();
  },

  ShowAvatar: function (hParticipant)
  {
    $('#' + arena.GetParticipantDomId(hParticipant)).css('display', '');
  },

  HideAvatar: function (hParticipant)
  {
    $('#' + arena.GetParticipantDomId(hParticipant)).css('display', 'none');
  },

  SetAvatarPosition: function (hParticipant, nX)
  {
    $('#' + arena.GetParticipantDomId(hParticipant)).css('left', nX + 'px');
  },

  SetAvatarNickname: function (hParticipant, sNickname)
  {
    $('#' + arena.GetParticipantDomId(hParticipant) + ' .cNickname').html(sNickname);
  },

  SetAvatarImage: function (hParticipant, sUrl)
  {
    var img = new Image()
    img.onload = function() {
      $('#' + arena.GetParticipantDomId(hParticipant) + ' .cImage').css('background-image', 'url(' + sUrl + ')');
    }
    img.src = sUrl;
  },

  AddAvatarChat: function (hParticipant, hChat, sText)
  {
    $('#' + arena.GetParticipantDomId(hParticipant) + ' .cChatContainer').append('<div id="' + arena.GetChatDomId(hParticipant, hChat) + '" class="cChat">' + sText + '</div>');
  },

  SetAvatarChat: function (hParticipant, hChat, sText)
  {
    $('#' + arena.GetChatDomId(hParticipant, hChat)).html(sText);
  },

  RemoveAvatarChat: function (hParticipant, hChat)
  {
    $('#' + arena.GetChatDomId(hParticipant, hChat)).remove();
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

  // --------------------------------------

  GetParticipantDomId: function (hParticipant)
  {
    return 'iP' + hParticipant.toDomCompatible();
  },

  GetChatDomId: function (hParticipant, hChat)
  {
    return 'iP' + hParticipant.toDomCompatible() + 'C' + hChat.toDomCompatible();
  },

  ActivateSelfAvatarElements: function (hParticipant)
  {
    $('.cImageSensor').hover(
      function () {
        $('.cMenu').stop(true).fadeTo('fast',1);
      }, 
      function () {
        $('.cMenu').fadeTo('fast',0);
      }
    );

    $('.cBubble').click(
      function () {
        $('.cMenu').fadeTo('fast', 0);
        $('.cChatIn').fadeIn('fast', 
          function() {
            $('.cChatIn .cText').focus();
          }
        );
      }
    );

    $('.cCloseButton').hover(
      function () {
        $(this).attr('src', 'CloseButtonHover.png');
      }, 
      function () {
        $(this).attr('src', 'CloseButton.png');
      }
    ).click(
      function () {
        arena.CloseChat();
      }
    );

    $('.cChatIn .cText').bind("keydown", 
      function(event) {
        var keycode = (event.keyCode ? event.keyCode : (event.which ? event.which : event.charCode));
        switch (keycode) {
        case 13:
          arena.SendPublicChat(this.value);
          this.value = '';
          return false; break;

        case 27:
          arena.CloseChat();
          return false; break;

        default:
          return true;
        }
      }
    );

    $('.cChatIn .cSend').click(
      function() {
        arena.SendPublicChat($('.cChatIn .cText').val());
        $('.cChatIn .cText').val('').focus();
      }
    );

  },

  // --------------------------------------

  SendPublicChat: function (sText)
  {
    api.Message('SendPublicChat').setString('ApType', 'WebArena_CallModuleSrpc').setString('sText', sText).send();
  },

  CloseChat: function ()
  {
    $('.cChatIn .cText').val('');
    $('.cChatIn').fadeOut('fast');
  },

  dummy: 1
}
