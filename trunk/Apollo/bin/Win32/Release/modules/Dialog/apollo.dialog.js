// ------------------------------------------
// Apollo Dialog Interface

ApSetContent = function(sUrl)
{
  $('#Content').attr('src', sUrl);
}

ApSetCaption = function(sCaption)
{
  $('#Caption').html(sCaption);
}

ApSetIcon = function(sUrl)
{
  $('#Icon').attr('src', sUrl);
}

ApContentEval = function(sCode)
{
  //api.Log.Debug('ApContentEval ' +sCode);
  return document.getElementById('Content').contentWindow.ApEval(sCode);
}

