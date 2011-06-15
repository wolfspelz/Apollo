// ------------------------------------------
// DialogInterface

function DialogApi()
{
}

DialogApi.prototype = 
{
  ModuleName: parent.api.ModuleName,
  Message: parent.api.Message,
  ModuleCall: parent.api.ModuleCall,
  Log: parent.api.Log,

  Ready: function()
  {
    parent.api.Message('Dialog_ContentLoaded').send();
  },

  _:0
}
