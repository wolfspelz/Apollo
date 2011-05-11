
function ShowAvatar(sId, sImage, sNickname, sPos)
{
  Log(' sId=' + sId + ' sImage=' + sImage + ' sNickname=' + sNickname + ' sPos=' + sPos);
  ui.logDebug('ShowAvatar');
}

function Arena(sContent)
{
  this.sContent = sContent;
}

Arena.prototype = {
  dummy: 1
}
