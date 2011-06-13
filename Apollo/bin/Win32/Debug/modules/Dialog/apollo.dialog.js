// ------------------------------------------
// DialogInterface

function DialogApi()
{
}

DialogApi.prototype = 
{
  ModuleName: parent.api.ModuleName,
  Message: parent.api.Message,
  Log: parent.api.Log,

  Ready: function()
  {
    parent.api.Message('Dialog_ContentLoaded').send();
  },

  _:0
}
