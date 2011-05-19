
function EscapeHTML(s)
{
  return s.replace(/&/ig, '&amp;').replace(/</ig, '&lt;').replace(/>/ig, '&gt;');
}

function StringApHandleBrackets(sValue)
{
  var sResult = '';
  if (sValue) {
    for (var i = 0; i < sValue.length; i++) {
	    switch (sValue.charAt(i)) {
		    case '[': break;
		    case ']': break;
		    default: sResult += sValue.charAt(i);
	    }
    }
  }
  return sResult;
}

function GetParticipantDomId(hParticipant)
{
  return 'iP' + StringApHandleBrackets(hParticipant);
}

function GetChatDomId(hParticipant, hChat)
{
  return 'iP' + StringApHandleBrackets(hParticipant) + 'C' + StringApHandleBrackets(hChat);
}

// -------------------------------------------

function Arena(sDomId)
{
  this.sDomId = sDomId;
}

Arena.prototype = {

  Startup: function()
  {
    //$('#' + arena.sDomId).html('<b>Arena</b>');
    $('#' + arena.sDomId).append(''
      + '<div id="iMeta">'
      + '  <div class="cTranslate">Hello World</div>'
      + '  <div id="iDocumentUrl"></div>'
      + '  <div id="iLocationUrl"></div>'
      + '  <div id="iLocationState"></div>'
      + '</div>'
    );
  },

  // --------------------------------------
  // Messages from module

  AddAvatar: function (hParticipant, bSelf, nX)
  {
    $('#' + arena.sDomId).append(''

    + '<div class="cParticipant" id="' + GetParticipantDomId(hParticipant) + '" style="display:none;">'
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
      + '          <td><img src="CloseChatInButton.png" class="cCloseButton" /></td>'
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
    $('#' + GetParticipantDomId(hParticipant)).remove();
  },

  ShowAvatar: function (hParticipant)
  {
    $('#' + GetParticipantDomId(hParticipant)).css('display', '');
  },

  HideAvatar: function (hParticipant)
  {
    $('#' + GetParticipantDomId(hParticipant)).css('display', 'none');
  },

  SetAvatarPosition: function (hParticipant, nX)
  {
    $('#' + GetParticipantDomId(hParticipant)).css('left', (50 + nX) + 'px');
  },

  SetAvatarNickname: function (hParticipant, sNickname)
  {
    $('#' + GetParticipantDomId(hParticipant) + ' .cNickname').html(EscapeHTML(sNickname));
  },

  SetAvatarImage: function (hParticipant, sUrl)
  {
    var img = new Image()
    img.onload = function() {
      $('#' + GetParticipantDomId(hParticipant) + ' .cImage').css('background-image', 'url(' + sUrl + ')');
    }
    img.src = sUrl;
  },

  AddAvatarChat: function (hParticipant, hChat, sText)
  {
    $('#' + GetParticipantDomId(hParticipant) + ' .cChatContainer').append(''
      + '<div id="' + GetChatDomId(hParticipant, hChat) + '" class="cChatBubble">' 
      + '<img src="CloseChatBubbleButton.png" class="cCloseButton" id="' + GetChatDomId(hParticipant, hChat) + '_Close" />'
      + '<span class="cText">'
      + EscapeHTML(sText)
      + '</span>'
      + '</div>'
    );

    $('#' + GetChatDomId(hParticipant, hChat))
      .click(
        function () {
          $('#' + GetChatDomId(hParticipant, hChat)).stop(true).fadeTo('fast', 1);
        }
      )
      .delay(10000)
      .fadeOut(20000, 
        function () {
          arena.OnPublicChatTimedOut(hParticipant, hChat);
        }
      );

    $('#' + GetChatDomId(hParticipant, hChat) + ' .cCloseButton').hover(
      function () {
        $(this).attr('src', 'CloseChatBubbleButtonHover.png');
      }, 
      function () {
        $(this).attr('src', 'CloseChatBubbleButton.png');
      }
    ).click(
//    (function (hParticipant, hChat) {
//        return function () {
//          arena.OnPublicChatClosed(hP, hC);
//        };
//      }) (hParticipant, hChat)
      function () {
        arena.OnPublicChatClosed(hParticipant, hChat);
      }
    );

  },

  SetAvatarChat: function (hParticipant, hChat, sText)
  {
    $('#' + GetChatDomId(hParticipant, hChat) + ' .cText').html(EscapeHTML(sText));
  },

  RemoveAvatarChat: function (hParticipant, hChat)
  {
    $('#' + GetChatDomId(hParticipant, hChat)).remove();
  },

  SetDocumentUrl: function (sUrl)
  {
    $('#iDocumentUrl').html(EscapeHTML(sUrl));
  },

  SetLocationUrl: function (sUrl)
  {
    $('#iLocationUrl').html(EscapeHTML(sUrl));
  },

  SetLocationState: function (sState)
  {
    $('#iLocationState').html(EscapeHTML(sState));
    $('#iLocationState').removeClass().addClass("sState");
  },

  // --------------------------------------
  // Messages to module
  
  OnPublicChatTimedOut: function (hParticipant, hChat)
  {
    api.Message('OnPublicChatTimedOut').setString('ApType', 'Arena_CallModuleSrpc').setString('hParticipant', hParticipant).setString('hChat', hChat).send();
  },

  OnPublicChatClosed: function (hParticipant, hChat)
  {
    api.Message('OnPublicChatClosed').setString('ApType', 'Arena_CallModuleSrpc').setString('hParticipant', hParticipant).setString('hChat', hChat).send();
  },

  SendPublicChat: function (sText)
  {
    api.Message('SendPublicChat').setString('ApType', 'Arena_CallModuleSrpc').setString('sText', sText).send();
  },

  // --------------------------------------
  // protected
  
  ActivateSelfAvatarElements: function (hParticipant)
  {
    $('.cImageSensor').hover(
      function () {
        $('.cMenu').stop(true).fadeTo('fast', 1);
      }, 
      function () {
        $('.cMenu').fadeTo('fast', 0);
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
        $(this).attr('src', 'CloseChatInButtonHover.png');
      }, 
      function () {
        $(this).attr('src', 'CloseChatInButton.png');
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

  CloseChat: function ()
  {
    $('.cChatIn .cText').val('');
    $('.cChatIn').fadeOut('fast');
  },

  // --------------------------------------

  dummy: 1
}
