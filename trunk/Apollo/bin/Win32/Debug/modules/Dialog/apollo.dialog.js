// ------------------------------------------
// Apollo Dialog Interface

ApSetContent = function(sUrl)
{
  $('#iApContent').attr('src', sUrl);
}

ApSetCaption = function(sCaption)
{
  $('#iApCaption').html(sCaption);
}

ApSetIcon = function(sUrl)
{
  $('#iApIcon').attr('src', sUrl);
}

ApGetContentRect = function () {
  var e = document.getElementById('iApContent');
  var nLeft = 0;
  var nTop = 0;
  var nWidth = 0;
  var nHeight = 0;
  while (e != null) {
    nLeft += e.offsetLeft;
    nTop += e.offsetTop;
    nWidth += e.offsetWidth;
    nHeight += e.offsetHeight;
    e = e.offsetParent;
  }

  var msg = new ApMessage();
  msg.setInt('nLeft', nLeft);
  msg.setInt('nTop', nTop);
  msg.setInt('nWidth', nWidth);
  msg.setInt('nHeight', nHeight);
  return msg.toString();
}

//ApContentEval = function(sCode)
//{
//  return document.getElementById('iApContent').contentWindow.ApEval(sCode);
//}

