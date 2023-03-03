#pragma once
#include <any>
#include <vector>
#include <exception>

struct sqlite3;

#if defined(_MSC_VER)
  typedef __int64 sqlite_int64;
  typedef unsigned __int64 sqlite_uint64;
#else
  typedef long long int sqlite_int64;
  typedef unsigned long long int sqlite_uint64;
#endif
typedef sqlite_int64 sqlite3_int64;
typedef sqlite_uint64 sqlite3_uint64;

inline long long    ANY_INT(const std::any& a) { return std::any_cast<sqlite3_int64>(a); }
inline double       ANY_REAL(const std::any& a) { return std::any_cast<double>(a); }
inline std::string  ANY_STR(const std::any& a) { return std::any_cast<std::string>(a); }


#define SQLITE_OK 0

class SQLite
{
private:
    mutable sqlite3* _db = NULL;
    mutable char lastSql[128]{ 0 };
    char tag[128]{ 0 };
    bool inTransaction = false;
public:
    SQLite() = delete;
    SQLite(const char* path, const char* tag = "UNNAMED");
    ~SQLite();

protected:
    std::vector<std::vector<std::any>> query(const char* stmt, size_t retSize, std::initializer_list<std::any> args = {}) const;
    int exec(const char* zsql, std::initializer_list<std::any> args = {});
    void commit();

    std::unordered_map<HashMD5, std::vector<std::vector<std::any>>> 
        queryMapHash(const char* stmt, size_t retSize, size_t keyIndex, std::initializer_list<std::any> args = {}) const;

public:
    void transactionStart();
    void transactionStop();
    void optimize();
    const char* errmsg() const;
};

