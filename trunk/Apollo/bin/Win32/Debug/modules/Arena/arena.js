
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

function LimitChat(sText, nMaxLength)
{
  if (nMaxLength == null) { nMaxLength = 256; }
  var sNewText = sText;
  sNewText = sText.substring(0, nMaxLength);
  if (sNewText.length != sText.length) {
    sNewText += '...';
  }
  return sNewText;
}
  
// -------------------------------------------

function Arena(sDomId)
{
  this.sDomId = sDomId;
}

Arena.prototype = {

  Startup: function()
  {
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

    + '<div class="cParticipant' 
    + ' ' + (bSelf ? 'cSelf' : 'cPeer')
    + '" id="' + GetParticipantDomId(hParticipant) + '" style="display:none;">'
    + '  <div class="cCenter">'
    + '    <div class="cChatWrapper" style="z-index:30;">'
    + '      <div class="cChatContainer"></div>'
    + '    </div>'
//    + (bSelf ? ''
//      + '  <div class="cPointer" style="z-index:100;">'
//      + '    <img class="cCloseButton" style="display:none;" src="img/CloseArrowButton.png" />'
//      + '  </div>'
//      :'')
    + '    <div class="cImageWrapper" style="z-index:30;">'
    + '      <div class="cImage" style="z-index:40;"></div>'
    + '      <div class="cNickname" style="z-index:50;">Long Nickname</div>'
    + '      <img class="cIcon" style="display:none; z-index:42;" />'
    + '      <img class="cCommunity" style="display:none; z-index:43;" />'
    + (bSelf ? ''
      + '    <div class="cChatIn" style="display:none; z-index:100;">'
      + '      <table border="0" cellpadding="0" cellspacing="0"><tr>'
      + '        <td><input type="text" class="cText" /></td>'
      + '        <td><input type="submit" class="cSend cTranslate" value="Send" /></td>'
      + '        <td><img src="img/CloseChatInButton.png" class="cCloseButton" /></td>'
      + '      </tr></table>'
      + '    </div>'
      :'')
    + '    </div>'
    + '  </div>'
    + '</div>'

    );

    arena.SetAvatarPosition(hParticipant, nX);
    arena.ShowAvatar(hParticipant);
    
    if (bSelf) {
      arena.ActivateSelfAvatarElements(hParticipant);
    } else {
      arena.ActivatePeerAvatarElements(hParticipant);
    }

    $('#' + GetParticipantDomId(hParticipant) + ' .cTranslate').each( function () { api.TranslateElement(this, null); } );
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
    $('#' + GetParticipantDomId(hParticipant)).css('left', nX + 'px');
  },

  MoveAvatarPosition: function (hParticipant, nX, nSpeedX)
  {
    var e = $('#' + GetParticipantDomId(hParticipant));

    var nOldX = parseInt(e.get(0).offsetLeft);
    
    var nDiff = nX - nOldX;
    if (nDiff < 0) { nDiff = -nDiff; };
    var nDuration = (nDiff * 1000) / nSpeedX;

    e .stop(true)
      .animate(
        { left: nX + 'px' },
        nDuration,
        'linear',
          function () {
            arena.OnAvatarPositionReached(hParticipant, nX);
          }
      );
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

  SetCommunityAttachment: function (hParticipant, sUrl, sLabel, sLink)
  {
    var e = $('#' + GetParticipantDomId(hParticipant) + ' .cCommunity');

    if (sUrl && sUrl != '') {
      e .attr('title', EscapeHTML(sLabel))
        .attr('src', sUrl)
        .css('display', '')
        .click(
          function () {
            arena.OnIconAttachmentClicked(hParticipant, sLink);
          }
        );
    } else {
      e.css('display', 'none');
    }
  },

  SetIconAttachment: function (hParticipant, sUrl, sLabel)
  {
    var e = $('#' + GetParticipantDomId(hParticipant) + ' .cIcon');

    if (sUrl && sUrl != '') {
      e .attr('title', EscapeHTML(sLabel))
        .attr('src', sUrl)
        .css('display', '');
    } else {
      e.css('display', 'none');
    }
  },

  SetOnlineStatus: function (hParticipant, sStatus)
  {
    var e = $('#' + GetParticipantDomId(hParticipant));
    
    if (sStatus == 'available' || sStatus == '' || sStatus == 'chat') {
      e.fadeTo('fast', 1);
    } else if (sStatus == 'away' || sStatus == 'xa' || sStatus == 'dnd') {
      e.fadeTo('fast', 0.5);
    }
  },

  AddAvatarChat: function (hParticipant, hChat, sText)
  {
    $('#' + GetParticipantDomId(hParticipant) + ' .cChatContainer').append(''
      + '<div id="' + GetChatDomId(hParticipant, hChat) + '" class="cChatBubble">' 
      + '<img src="img/CloseChatBubbleButton.png" class="cCloseButton" id="' + GetChatDomId(hParticipant, hChat) + '_Close" />'
      + '<span class="cText">'
      + EscapeHTML(LimitChat(sText))
      + '</span>'
      + '</div>'
    );

    $('#' + GetChatDomId(hParticipant, hChat))
      .click(
        function () {
          $('#' + GetChatDomId(hParticipant, hChat)).stop(true).fadeTo('fast', 1);
        }
      )
      .delay(30000)
      .fadeOut(30000, 
        function () {
          arena.OnPublicChatTimedOut(hParticipant, hChat);
        }
      );

    $('#' + GetChatDomId(hParticipant, hChat) + ' .cCloseButton').hover(
      function () {
        $(this).attr('src', 'img/CloseChatBubbleButtonHover.png');
      }, 
      function () {
        $(this).attr('src', 'img/CloseChatBubbleButton.png');
      }
    ).click(
      // Example for closure executed now
      //(function (hParticipant, hChat) {
      //    return function () {
      //      arena.OnPublicChatClosed(hP, hC);
      //    };
      //  }) (hParticipant, hChat)
      function () {
        arena.OnPublicChatClosed(hParticipant, hChat);
      }
    );

  },

  SetAvatarChat: function (hParticipant, hChat, sText)
  {
    sText = sText.substring(0, 300);
    $('#' + GetChatDomId(hParticipant, hChat) + ' .cText').html(EscapeHTML(LimitChat(sText)));
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

  OnIconAttachmentClicked: function (hParticipant, sLink)
  {
    api.Message('OnIconAttachmentClicked').setString('ApType', 'Arena_CallModuleSrpc').setString('hParticipant', hParticipant).setString('sLink', sLink).send();
  },

  OnAvatarPositionReached: function (hParticipant, nX)
  {
    api.Message('OnAvatarPositionReached').setString('ApType', 'Arena_CallModuleSrpc').setString('hParticipant', hParticipant).setInt('nX', nX).send();
  },

  OnAvatarDraggedBy: function (hParticipant, nX, nY)
  {
    api.Message('OnAvatarDraggedBy').setString('ApType', 'Arena_CallModuleSrpc').setString('hParticipant', hParticipant).setInt('nX', nX).setInt('nY', nY).send();
  },

  OnAvatarPointerClosed: function (hParticipant)
  {
    api.Message('OnAvatarPointerClosed').setString('ApType', 'Arena_CallModuleSrpc').setString('hParticipant', hParticipant).send();
  },

  SendPublicChat: function (sText)
  {
    api.Message('SendPublicChat').setString('ApType', 'Arena_CallModuleSrpc').setString('sText', sText).send();
  },

  // --------------------------------------
  // protected
    
  ActivateSelfAvatarElements: function (hParticipant)
  {
    $('.cChatIn .cCloseButton').hover(
      function () {
        $(this).attr('src', 'img/CloseChatInButtonHover.png');
      }, 
      function () {
        $(this).attr('src', 'img/CloseChatInButton.png');
      }
    ).click(
      function (ev) {
        ev.stopPropagation();
        arena.CloseChat();
      }
    );

//    $('.cPointer').hover(
//      function () {
//        $('.cPointer .cCloseButton').fadeIn('fast');
//      }, 
//      function () {
//        $('.cPointer .cCloseButton').fadeOut('fast');
//      }
//    ).click(
//      function (ev) {
//        ev.stopPropagation();
//      }
//    );

//    $('.cPointer .cCloseButton').hover(
//      function () {
//        $(this).attr('src', 'img/CloseArrowButtonHover.png');
//      }, 
//      function () {
//        $(this).attr('src', 'img/CloseArrowButton.png');
//      }
//    ).click(
//      function (ev) {
//        ev.stopPropagation();
//        $('.cPointer').fadeOut('fast');
//        arena.OnAvatarPointerClosed();
//      }
//    );

    $('.cChatIn .cText').bind("keydown", 
      function(ev) {
        var keycode = (ev.keyCode ? ev.keyCode : (ev.which ? ev.which : ev.charCode));
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
    ).click(
      function(ev) {
        ev.stopPropagation();
      }
    );

    $('.cChatIn .cSend').click(
      function(ev) {
        ev.stopPropagation();
        arena.SendPublicChat($('.cChatIn .cText').val());
        $('.cChatIn .cText').val('').focus();
      }
    );

    $('.cChatIn').click(
      function(ev) {
        ev.stopPropagation();
      }
    );

    $('#' + GetParticipantDomId(hParticipant)).draggable(
      {
        containment: '#' + arena.sDomId,
        scroll: false,
        stack: '.cParticipant',
        opacity: 0.7, 
        helper: "clone",
        handle: '.cImage',
        stop: function(ev, ui) {
          arena.OnAvatarDraggedBy(hParticipant, (ui.position.left - ui.originalPosition.left), (ui.position.top - ui.originalPosition.top));
        },
      }
    ).click(
      function() {

        if ($('.cChatIn').css('display') == 'none') {
          $('.cChatIn').fadeIn('fast', 
            function() {
              $('.cChatIn .cText').focus();
            }
          );
        } else {
          $('.cChatIn').fadeOut('fast');
        }

      }
    );
    
  },

  ActivatePeerAvatarElements: function (hParticipant)
  {
    $('#' + GetParticipantDomId(hParticipant)).draggable(
      {
        containment: '#' + arena.sDomId,
        scroll: false,
        stack: '.cParticipant',
        handle: '.cImage',
        stop: function(ev, ui) {
          $(this).css('bottom', 0 - (ui.position.top - ui.originalPosition.top) + 'px');
          $(this).css('top', '');
          $(this).animate( { bottom: '0px' }, 800 );
        },
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
