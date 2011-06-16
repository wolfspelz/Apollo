// ------------------------------------------
// Apollo Dialog Interface

ApSetContent = function(sUrl)
{
  $('#Content').attr('src', sUrl);
}

ApShowContent = function(bVisible)
{
  if (bVisible) {
    $('#Content').css('display', '');
  } else {
    $('#Content').css('display', 'none');
  }
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

