#pragma once
#include <any>
#include <vector>
#include <exception>

struct sqlite3;

#if defined(_MSC_VER) || defined(__BORLANDC__)
  typedef __int64 sqlite_int64;
  typedef unsigned __int64 sqlite_uint64;
#else
  typedef long long int sqlite_int64;
  typedef unsigned long long int sqlite_uint64;
#endif
typedef sqlite_int64 sqlite3_int64;
typedef sqlite_uint64 sqlite3_uint64;


#define SQLITE_OK 0

class SQLite
{
private:
    mutable sqlite3* _db;
    char tag[128];
    bool inTransaction = false;
public:
    SQLite() = delete;
    SQLite(const char* path, const char* tag = "UNNAMED");
    ~SQLite();

protected:
    std::vector<std::vector<std::any>> query(const char* stmt, size_t retSize, std::initializer_list<std::any> args = {}) const;
    int exec(const char* zsql, std::initializer_list<std::any> args = {});
    void commit();

public:
    void transactionStart();
    void transactionStop();
    const char* errmsg();
};

