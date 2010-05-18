// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(SQLite_H_INCLUDED)
#define SQLite_H_INCLUDED

#include "SQLite3/sqlite3.h"

class SQLiteStatement;

class SQLiteFile: public Elem
{
public:
  SQLiteFile(const String& sName, const String& sFilePath);
  virtual ~SQLiteFile();

  int isOpen();
  sqlite3* getSQLiteHandle() { return pDb_; }
  String getErrorMessage(); 
  String getFilePath() { return sFilePath_; }
  int isChanged() { return bChanged_; }

  static String pathFromName(const String& sName);

  int open();
  int close();
  int check();
  int clear();
  int expire();
  int deleteOlderThan(int nAge);
  int sync();
  int createSchema();
  int deleteSchema();
  int migrateSchema(int nFrom, int nTo);
  int beginTransaction();
  int commitTransaction();

  int setValue(const String& sKey, String& sValue, int nLifetime);
  int setValue(const String& sKey, const unsigned char* pData, size_t nLength, int nLifetime);
  int deleteValue(const String& sKey);
  int getValue(const String& sKey, String& sValue);
  int getValue(const String& sKey, Buffer& sbValue);
  int getKeys(Apollo::ValueList& vlKeys);

protected:
  SQLiteStatement* prepareStatement(const String& sSql);
  void exec(const String& sSql);
  bool tableExists(const String& sTable);

protected:
  String sFilePath_;
  sqlite3* pDb_;
  int bChanged_;
  int bTransaction_;
  time_t tCompactTime_;
};

class SQLiteValue: public Elem
{
public:
  SQLiteValue(const String& sName, int nType)
    :Elem(sName)
    ,nType_(nType)
  {}

  void setType(int nType) { nType_ = nType; }

  void setString(const char* szStr) { Elem::setString(szStr); }
  String& getString();

protected:
  int nType_;
  SQLiteValue() {}
  friend class ListT<SQLiteValue, Elem>;
};

typedef ListT<SQLiteValue, Elem> SQLiteRow;

class SQLiteStatement
{
public:
  SQLiteStatement(SQLiteFile& db, sqlite3_stmt *pStmt)
    :db_(db)
    ,pStmt_(pStmt)
  {}
  virtual ~SQLiteStatement();

  void bindText(int nPos, const char* szText, int nLength);
  void bindBlob(int nPos, const unsigned char* pData, int nLength);
  void bindInt(int nPos, int nInt);

  SQLiteRow* fetch();

protected:
  SQLiteFile& db_;
  sqlite3_stmt *pStmt_;
};

class SQLiteException
{
public:
  SQLiteException(int nRet, const char* szMethod, const char* szError)
    :sMethod_(szMethod)
    ,sMessage_(szError)
  {}

  String& getMethod() { return sMethod_; }
  String& getMessage() { return sMessage_; }

protected:
  String sMethod_;
  String sMessage_;
};

#endif // SQLite_H_INCLUDED
