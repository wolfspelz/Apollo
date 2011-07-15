// ============================================================================
//
// libvpi
//
// ============================================================================

static VPI_UNUSED_FUNCTION char** vpi_ListKeyValue_to_strlist(List& lList)
{
  char** result = 0;
  char** list = (char**) vpi_malloc((lList.length()+1) * sizeof(char*) * 2); // * 2 for key and value
  if (list != 0) {
    result = list;
    for (Elem* e = 0; (e = lList.Next(e)); ) {
      *list = (char*) vpi_malloc(e->getName().bytes() + 1);
      ::memcpy(*list, e->getName().c_str(), e->getName().bytes() + 1);
      list++;
      *list = (char*) vpi_malloc(e->getString().bytes() + 1);
      ::memcpy(*list, e->getString().c_str(), e->getString().bytes() + 1);
      list++;
    }
    *list = 0;
  }
  return result;
}

static VPI_UNUSED_FUNCTION char** vpi_ListKey_to_strlist(List& lList)
{
  char** result = 0;
  char** list = (char**) vpi_malloc((lList.length()+1) * sizeof(char*));
  if (list != 0) {
    result = list;
    for (Elem* e = 0; (e = lList.Next(e)); ) {
      *list = (char*) vpi_malloc(e->getName().bytes() + 1);
      ::memcpy(*list, e->getName().c_str(), e->getName().bytes() + 1);
      list++;
    }
    *list = 0;
  }
  return result;
}

static VPI_UNUSED_FUNCTION void vpi_strlist_to_ListKeyValue(char** strlist, List& lList)
{
  char** p = strlist;
  while (*p != 0) {
    String sKey = *p;
    p++;
    String sValue = *p;
    p++;
    lList.AddLast(sKey, sValue);
  }
}

static VPI_UNUSED_FUNCTION void vpi_strlist_to_ListKey(char** strlist, List& lList)
{
  char** p = strlist;
  while (*p != 0) {
    String sKey = *p;
    p++;
    lList.AddLast(sKey);
  }
}
