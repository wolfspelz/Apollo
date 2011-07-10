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

//ApContentEval = function(sCode)
//{
//  return document.getElementById('iApContent').contentWindow.ApEval(sCode);
//}

