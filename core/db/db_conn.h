#pragma once
#include <any>
#include <vector>
#include <exception>

struct sqlite3;

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

    std::vector<std::vector<std::any>> query(const char* stmt, size_t retSize) const;
    std::vector<std::vector<std::any>> query(const char* stmt, size_t retSize, std::initializer_list<std::any> args) const;
    int exec(const char* zsql);
    int exec(const char* zsql, std::initializer_list<std::any> args);

    void transactionStart();
    void transactionStop();
    void commit(bool restart_transaction = false);
    const char* errmsg();
};

