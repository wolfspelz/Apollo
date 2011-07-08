// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "SQLite.h"
#include "SAutoPtr.h"

//----------------------------------------------------------
// SQLiteValue

String& SQLiteValue::getString()
{
  switch (nType_) {
    case SQLITE_BLOB: {
      size_t nDataLen = 0;
      char* pDataPtr = 0;
      if (readData(&nDataLen, &pDataPtr)) {
        sString_.set(pDataPtr, nDataLen);
      }
    } break;

    case SQLITE_INTEGER: {
      sString_ = String::from(getInt());
    } break;
  }

  return sString_;
}

//----------------------------------------------------------
// SQLiteStatement

SQLiteStatement::~SQLiteStatement()
{
  if (pStmt_ != 0) {
		int nRet = sqlite3_finalize(pStmt_);
    if (nRet != SQLITE_OK) {
      //throw SQLiteException(nRet, LOG_CONTEXT, db_.getErrorMessage());
    }
    pStmt_ = 0;
  }
}

void SQLiteStatement::bindText(int nPos, const char* szText, int nLength)
{
  if (pStmt_ == 0) { throw SQLiteException(SQLITE_ERROR, LOG_CONTEXT, "No statement"); }

  int nRet = sqlite3_bind_text(pStmt_, nPos, szText, nLength, SQLITE_TRANSIENT);
  if (nRet != SQLITE_OK) {
    throw SQLiteException(nRet, LOG_CONTEXT, db_.getErrorMessage());
  }
}

void SQLiteStatement::bindBlob(int nPos, const unsigned char* pData, int nLength)
{
  if (pStmt_ == 0) { throw SQLiteException(SQLITE_ERROR, LOG_CONTEXT, "No statement"); }

  int nRet = sqlite3_bind_blob(pStmt_, 2, pData, nLength, SQLITE_TRANSIENT);
  if (nRet != SQLITE_OK) {
    throw SQLiteException(nRet, LOG_CONTEXT, db_.getErrorMessage());
  }
}

void SQLiteStatement::bindInt(int nPos, int nInt)
{
  if (pStmt_ == 0) { throw SQLiteException(SQLITE_ERROR, LOG_CONTEXT, "No statement"); }

  int nRet = sqlite3_bind_int(pStmt_, nPos, nInt);
  if (nRet != SQLITE_OK) {
    throw SQLiteException(nRet, LOG_CONTEXT, db_.getErrorMessage());
  }
}

SQLiteRow* SQLiteStatement::fetch()
{
  SQLiteRow* pRow = 0;

  if (pStmt_ == 0) { throw SQLiteException(SQLITE_ERROR, LOG_CONTEXT, "No statement"); }

  int nRet = sqlite3_step(pStmt_);
  if (nRet == SQLITE_ROW){
    pRow = new SQLiteRow();
    if (pRow != 0) {
      int nColumns = sqlite3_column_count(pStmt_);
      for (int iCol = 0; iCol < nColumns; ++iCol) {

        int nType = sqlite3_column_type(pStmt_, iCol);
        String sName;
        sName.appendf("%d", iCol);
        SQLiteValue* pValue = new SQLiteValue(sName, nType);
        if (pValue != 0) {

          switch (nType) {
          case SQLITE_INTEGER:
              pValue->setInt(sqlite3_column_int(pStmt_, iCol));
            break;
          case SQLITE_TEXT:
            {
              const char * szText = (const char *) sqlite3_column_text(pStmt_, iCol);
              if (szText != 0) {
                pValue->setString(szText);
              } else {
                pValue->setType(SQLITE_NULL);
              }
            }
            break;
          case SQLITE_BLOB:
            {
              const void* pData = sqlite3_column_blob(pStmt_, iCol);
              if (pData != 0) {
                int nSize = sqlite3_column_bytes(pStmt_, iCol);
                pValue->writeData(nSize, (char *) pData);
              } else {
                pValue->setType(SQLITE_NULL);
              }
            }
            break;
          default:
            {
              String s;
              s.appendf("Unknown type: %d", nType);
              throw SQLiteException(nRet, LOG_CONTEXT, s);
            } // default
          } // switch (nType)

          pRow->AddLast(pValue);

        } // if (pValue != 0)
      } // for (int iCol = 0...
    } // if (pRow != 0)

  } else if (nRet == SQLITE_DONE) {
    // do nothing
  } else {
    nRet = sqlite3_reset(pStmt_);
    throw SQLiteException(nRet, LOG_CONTEXT, db_.getErrorMessage());
  }

  return pRow;
}

//----------------------------------------------------------
// SQLiteFile

SQLiteFile::SQLiteFile(const String& sName, const String& sFilePath)
:Elem(sName)
,sFilePath_(sFilePath)
,pDb_(0)
,bChanged_(0)
,bTransaction_(0)
,tCompactTime_(Apollo::getNow().Sec())
{
}

SQLiteFile::~SQLiteFile()
{
  if (isOpen()) {
    (void) close();
  }
}

String SQLiteFile::getErrorMessage()
{
  String sError = sqlite3_errmsg(pDb_);

  if (isOpen()) {
    sError = sqlite3_errmsg(pDb_);
  }

  return sError;
}

String SQLiteFile::pathFromName(const String& sName)
{
  String sBase = Apollo::getModuleConfig(MODULE_NAME, "BaseFolder", Apollo::getAppResourcePath());
  sBase = Apollo::applyPathVars(sBase);
  if (!sBase.empty()) { sBase.makeTrailingSlash(); }

  String sFile = sName;
  sFile.escape(String::EscapeURL);

  String sPathName = sBase + sFile + ".sqlite3";

  return sPathName;
}

int SQLiteFile::isOpen()
{
  return (pDb_ != 0);
}

int SQLiteFile::open()
{
  int ok = 0;

  if (isOpen()) {
    (void) close();
  }

  int nRet = sqlite3_open(sFilePath_, &pDb_);
	if (nRet != SQLITE_OK) {
		String sError = sqlite3_errmsg(pDb_);
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "sqlite3_open(%s) failed: %s", _sz(sFilePath_), _sz(sError)));
    (void) close();
  } else {
    ok = 1;
  }

  return ok;
}

int SQLiteFile::close()
{
  int ok = 0;

  if (pDb_ != 0) {
    /*
    sqlite3_stmt *pStmt = 0;
    while( (pStmt = sqlite3_next_stmt(pDb_, pStmt)) != 0){
      int nRet = sqlite3_finalize(pStmt);
	    if (nRet != SQLITE_OK) {
		    String sError = sqlite3_errmsg(pDb_);
        apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "sqlite3_finalize() failed: %s", _sz(sError)));
      }
    }
    */
    int nRet = sqlite3_close(pDb_);
	  if (nRet != SQLITE_OK) {
		  String sError = sqlite3_errmsg(pDb_);
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "sqlite3_close(%s) failed: %s", _sz(sFilePath_), _sz(sError)));
    } else {
      ok = 1;
	  }
    pDb_ = 0;
  }

  return ok;
}

int SQLiteFile::beginTransaction()
{
  int ok = 1;

  if (isOpen()) {
    try {
      exec("BEGIN TRANSACTION;");
      bTransaction_ = 1;

    } catch (SQLiteException& ex) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
      ok = 0;
    }
  }

  return ok;
}

int SQLiteFile::commitTransaction()
{
  int ok = 1;

  if (isOpen() && bTransaction_) {
    try {
      exec("COMMIT TRANSACTION;");
      bTransaction_ = 0;

    } catch (SQLiteException& ex) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
      ok = 0;
    }
  }

  return ok;
}

int SQLiteFile::check()
{
  int ok = 0;

  if (isOpen()) {
    int kVersion = DB_VERSION;
    int nVersion = 0;

    try {
      if (tableExists("Version")) {
        AutoPtr<SQLiteStatement> pStmt(prepareStatement("SELECT nVersion FROM Version;"));
        AutoPtr<SQLiteRow> pRow(pStmt->fetch());
        if (pRow != 0) {
          nVersion = (*pRow)["0"].getInt();
        }
      } // tableExists

    } catch (SQLiteException& ex) {
      apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
      ok = 0;
    }

    if (nVersion < kVersion) {
      apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "old version, need=%d found=%d", kVersion, nVersion));

      ok = migrateSchema(nVersion, kVersion);
      if (!ok) {
        apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "MigrateSchema(%d, %d) failed", nVersion, kVersion));
      }
    } else if (nVersion > kVersion) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "version too new, need=%d found=%d", kVersion, nVersion));
    } else {
      ok = 1;
      apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "version ok: %d", nVersion));
    }
  }

  return ok;
}

int SQLiteFile::createSchema()
{
  int ok = 0;

  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Creating DB schema (%s)", _sz(sFilePath_)));

  try {
    exec("CREATE TABLE Data (sKey TEXT UNIQUE, oValue BLOB, tCreated INT UNSIGNED, tLifetime INT UNSIGNED);");
    exec("CREATE TABLE Version (nVersion INTEGER);");
    exec("CREATE UNIQUE INDEX iKey ON Data(sKey ASC);");
    { String sSql; sSql.appendf("INSERT INTO Version VALUES (%d);", DB_VERSION); exec(sSql); }
    ok = 1;

  } catch (SQLiteException& ex) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
    ok = 0;
  }

  if (ok) {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Creating DB schema done (%s)", _sz(sFilePath_)));
  } else {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Creating DB schema failed (%s)", _sz(sFilePath_)));
  }

  return ok;
}

int SQLiteFile::deleteSchema()
{
  int ok = 0;

  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Creating DB schema (%s)", _sz(sFilePath_)));

  try {
    if (tableExists("Data")) {
      exec("DROP TABLE Data;");
    }
    if (tableExists("Version")) {
      exec("DROP TABLE Version;");
    }
    ok = 1;

  } catch (SQLiteException& ex) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
    ok = 0;
  }

  if (ok) {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Deleting DB schema done (%s)", _sz(sFilePath_)));
  } else {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Deleting DB schema failed (%s)", _sz(sFilePath_)));
  }

  return ok;
}

int SQLiteFile::migrateSchema(int nFrom, int nTo)
{
  int ok = 1;

  // should migrate the data
  (void) deleteSchema();
  ok = createSchema();

  return ok;
}

int SQLiteFile::clear()
{
  int ok = 1;

  (void) deleteSchema();
  ok = createSchema();

  return ok;
}

int SQLiteFile::expire()
{
  int ok = 1;

  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Expire DB (%s)", _sz(sFilePath_)));

  try {
    time_t tNow = Apollo::getNow().Sec();
    String sSql;
    sSql.appendf("DELETE FROM Data WHERE tLifetime > 0 AND tCreated + tLifetime <= %d;", tNow);
    exec(sSql);
    ok = 1;

  } catch (SQLiteException& ex) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
    ok = 0;
  }

  if (!ok) {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Expire DB failed (%s)", _sz(sFilePath_)));
  }

  return ok;
}

int SQLiteFile::deleteOlderThan(int nAge)
{
  int ok = 1;

  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Expire DB (%s)", _sz(sFilePath_)));

  try {
    time_t tNow = Apollo::getNow().Sec();
    String sSql;
    sSql.appendf("DELETE FROM Data WHERE tCreated <= %d;", tNow - nAge);
    exec(sSql);
    ok = 1;

  } catch (SQLiteException& ex) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
    ok = 0;
  }

  if (!ok) {
    apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Expire DB failed (%s)", _sz(sFilePath_)));
  }

  return ok;
}

int SQLiteFile::sync()
{
  int ok = 1;

  commitTransaction();
  bChanged_ = 0;

  time_t tNow = Apollo::getNow().Sec();
  if (tNow - tCompactTime_ > 3600) {
    tCompactTime_ = tNow;
    try {
      exec("VACUUM;");
    } catch (SQLiteException& ex) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "compact db: %s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
    }
  }

  ok = beginTransaction();

  return ok;
}

SQLiteStatement* SQLiteFile::prepareStatement(const String& sSql)
{
  SQLiteStatement* pResult = 0;

  if (!isOpen()) { throw SQLiteException(SQLITE_ERROR, LOG_CONTEXT, "File not open"); }

  sqlite3_stmt *pStmt = 0;
  int nRet = sqlite3_prepare(pDb_, sSql, -1, &pStmt, 0);
  if (nRet != SQLITE_OK || pStmt == 0) {
    throw SQLiteException(nRet, LOG_CONTEXT, getErrorMessage());
  } else {
    pResult = new SQLiteStatement(*this, pStmt);
  }

  return pResult;
}

bool SQLiteFile::tableExists(const String& sTable)
{
  int bExists = false;

  if (isOpen()) {
    try {
      AutoPtr<SQLiteStatement> pStmt(prepareStatement("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name=?"));
      pStmt->bindText(1, sTable.c_str(), sTable.bytes());
      AutoPtr<SQLiteRow> pRow(pStmt->fetch());
      if (pRow != 0) {
        bExists = ((*pRow)["0"].getInt() == 1);
      }

    } catch (SQLiteException& ex) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
    }
  }

  return bExists;
}

int SQLiteFile::setValue(const String& sKey, String& sValue, int nLifetime)
{
  int ok = 0;

  if (isOpen()) {
    ok = 1;

    try {
      {
        AutoPtr<SQLiteStatement> pStmtInsert(prepareStatement("INSERT OR REPLACE INTO Data (sKey, oValue, tCreated, tLifetime) VALUES (?, ?, ?, ?);"));
        pStmtInsert->bindText(1, sKey.c_str(), sKey.bytes());
        pStmtInsert->bindText(2, sValue.c_str(), sValue.bytes());
        pStmtInsert->bindInt(3, Apollo::getNow().Sec());
        pStmtInsert->bindInt(4, nLifetime);
        AutoPtr<SQLiteRow> pRow(pStmtInsert->fetch());
      }

      bChanged_ = 1;

    } catch (SQLiteException& ex) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
      ok = 0;
    }
  }

  return ok;
}

int SQLiteFile::setValue(const String& sKey, const unsigned char* pData, size_t nLength, int nLifetime)
{
  int ok = 0;

  if (isOpen()) {
    ok = 1;

    try {
      {
        AutoPtr<SQLiteStatement> pStmtInsert(prepareStatement("INSERT OR REPLACE INTO Data (sKey, oValue, tCreated, tLifetime) VALUES (?, ?, ?, ?);"));
        pStmtInsert->bindText(1, sKey.c_str(), sKey.bytes());
        pStmtInsert->bindBlob(2, pData, nLength);
        pStmtInsert->bindInt(3, Apollo::getNow().Sec());
        pStmtInsert->bindInt(4, nLifetime);
        AutoPtr<SQLiteRow> pRow(pStmtInsert->fetch());
      }

      bChanged_ = 1;

    } catch (SQLiteException& ex) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
      ok = 0;
    }
  }

  return ok;
}

int SQLiteFile::deleteValue(const String& sKey)
{
  int ok = 0;

  if (isOpen()) {
    ok = 1;

    try {
      AutoPtr<SQLiteStatement> pStmt(prepareStatement("DELETE FROM Data WHERE sKey=?;"));
      pStmt->bindText(1, sKey.c_str(), sKey.bytes());
      AutoPtr<SQLiteRow> pRow(pStmt->fetch());

    } catch (SQLiteException& ex) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
      ok = 0;
    }
  }

  return ok;
}

int SQLiteFile::hasValue(const String& sKey, int& bAvailable)
{
  int ok = 0;

  if (isOpen()) {
    ok = 1;

    try {
      AutoPtr<SQLiteStatement> pStmt(prepareStatement("SELECT COUNT(*) FROM Data WHERE sKey=?;"));
      pStmt->bindText(1, sKey.c_str(), sKey.bytes());
      AutoPtr<SQLiteRow> pRow(pStmt->fetch());
      if (pRow != 0) {
        int nCount = (*pRow)["0"].getInt();
        if (nCount > 0) {
          bAvailable = 1;
        }
      }

    } catch (SQLiteException& ex) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
      ok = 0;
    }
  }

  return ok;
}

int SQLiteFile::getValue(const String& sKey, String& sValue)
{
  int ok = 0;

  if (isOpen()) {
    ok = 1;

    try {
      AutoPtr<SQLiteStatement> pStmt(prepareStatement("SELECT oValue FROM Data WHERE sKey=?;"));
      pStmt->bindText(1, sKey.c_str(), sKey.bytes());
      AutoPtr<SQLiteRow> pRow(pStmt->fetch());
      if (pRow != 0) {
        sValue = (*pRow)["0"].getString();
      }

    } catch (SQLiteException& ex) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
      ok = 0;
    }
  }

  return ok;
}

int SQLiteFile::getValue(const String& sKey, Buffer& sbValue)
{
  int ok = 0;

  if (isOpen()) {
    ok = 1;

    try {
      AutoPtr<SQLiteStatement> pStmt(prepareStatement("SELECT oValue FROM Data WHERE sKey=?;"));
      pStmt->bindText(1, sKey.c_str(), sKey.bytes());
      AutoPtr<SQLiteRow> pRow(pStmt->fetch());
      if (pRow != 0) {
        // use and delete row

        SQLiteValue* pValue = pRow->First();
        if (pValue != 0) {
          size_t nDataLen = 0;
          char* pDataPtr = 0;
          if (pValue->readData(&nDataLen, &pDataPtr)) {
            sbValue.SetData(pDataPtr, nDataLen);
          }
        } else {
          ok = 0;
        }
      }

    } catch (SQLiteException& ex) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
      ok = 0;
    }
  }

  return ok;
}

int SQLiteFile::getKeys(Apollo::ValueList& vlKeys)
{
  int ok = 0;

  if (isOpen()) {
    ok = 1;

    try {
      AutoPtr<SQLiteStatement> pStmt(prepareStatement("SELECT sKey FROM Data;"));
      int bDone = 0;
      while (!bDone) {
        AutoPtr<SQLiteRow> pRow(pStmt->fetch());
        if (pRow == 0) {
          bDone = 1;
        } else {
          vlKeys.add((*pRow)["0"].getString());
        } // pRow
      } // while !bDone

    } catch (SQLiteException& ex) {
      apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s failed: %s (%s)", _sz(ex.getMethod()), _sz(ex.getMessage()), _sz(sFilePath_)));
      ok = 0;
    }
  }

  return ok;
}

void SQLiteFile::exec(const String& sSql)
{
  if (!isOpen()) { throw SQLiteException(SQLITE_ERROR, LOG_CONTEXT, "File not open"); }

  int nRet = sqlite3_exec(pDb_, sSql, 0, 0, 0);
  if (nRet != SQLITE_OK) {
    throw SQLiteException(nRet, LOG_CONTEXT, getErrorMessage());
  }
}
