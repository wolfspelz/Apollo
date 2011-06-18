// ------------------------------------------
// Apollo Dialog Content Interface

function ApDialogContent()
{
}

ApDialogContent.prototype = 
{
  ModuleName: parent.api.ModuleName,
  Message: parent.api.Message,
  ModuleCall: parent.api.ModuleCall,
  Log: parent.api.Log,
  TranslateElement: parent.api.TranslateElement,
  Translate: parent.api.Translate,

  Ready: function()
  {
    parent.api.Message('Dialog_ContentLoaded').send();
  },

  _:0
}
