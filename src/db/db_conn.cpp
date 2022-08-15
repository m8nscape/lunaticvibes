#include <string>
#include <filesystem>
#include "sqlite3.h"
#include "db_conn.h"
#include "common/log.h"

#ifdef _MSC_VER
#ifndef strcpy
#define strcpy strcpy_s
#endif
#ifndef strncpy
#define strncpy strncpy_s
#endif
#endif

SQLite::SQLite(const char* path, const char* tag) 
{
    sqlite3_open(path, &_db); 
    strcpy(this->tag, sizeof(this->tag), tag);
}

SQLite::~SQLite() { sqlite3_close(_db); }
const char* SQLite::errmsg() const { return sqlite3_errmsg(_db); }

std::vector<std::vector<std::any>> SQLite::query(const char* zsql, size_t retSize, std::initializer_list<std::any> args) const
{
    memset(lastSql, 0, sizeof(lastSql));
    strncpy(lastSql, zsql, sizeof(lastSql) - 1);

    size_t argc = args.size();

    sqlite3_stmt* stmt = nullptr;
    const char* pzTail;
    if (int ret = sqlite3_prepare_v3(_db, zsql, (int)strlen(zsql), 0, &stmt, &pzTail))
    {
        LOG_ERROR << "[sqlite3] sql \"" << zsql << "\" prepare error: [" << ret << "] " << errmsg();
        return {};
    }

    int i = 1;
    for (auto& a : args)
    {
        if (a.type() == typeid(int)) sqlite3_bind_int(stmt, i, std::any_cast<int>(a));
        else if (a.type() == typeid(bool)) sqlite3_bind_int(stmt, i, int(std::any_cast<bool>(a)));
        else if (a.type() == typeid(long long)) sqlite3_bind_int64(stmt, i, std::any_cast<long long>(a));
        else if (a.type() == typeid(time_t)) sqlite3_bind_int64(stmt, i, std::any_cast<time_t>(a));
        else if (a.type() == typeid(double)) sqlite3_bind_double(stmt, i, std::any_cast<double>(a));
        else if (a.type() == typeid(std::string)) sqlite3_bind_text(stmt, i, std::any_cast<std::string>(a).c_str(), (int)std::any_cast<std::string>(a).length(), SQLITE_TRANSIENT);
        else if (a.type() == typeid(const char*)) sqlite3_bind_text(stmt, i, std::any_cast<const char*>(a), (int)strlen(std::any_cast<const char*>(a)), SQLITE_TRANSIENT);
        else if (a.type() == typeid(nullptr)) sqlite3_bind_null(stmt, i);
        else assert(false); // type error
        ++i;
    }

    std::vector<std::vector<std::any>> ret;
    size_t idx = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        ret.push_back({});
        ret[idx].resize(retSize);
        for (int i = 0; i < retSize; ++i)
        {
            auto c = sqlite3_column_type(stmt, i);
            if (SQLITE_INTEGER == c)
                ret[idx][i] = sqlite3_column_int64(stmt, i);
            else if (SQLITE_FLOAT == c)
                ret[idx][i] = sqlite3_column_double(stmt, i);
            else if (SQLITE_TEXT == c)
                ret[idx][i] = std::make_any<std::string>((const char*)sqlite3_column_text(stmt, i));
        }
        ++idx;
    }
    LOG_DEBUG << "[sqlite3] " << tag << ": " << " query " << zsql << " result: " << ret.size() << " rows";
    sqlite3_finalize(stmt);
    return ret;
}

int SQLite::exec(const char* zsql, std::initializer_list<std::any> args)
{
    memset(lastSql, 0, sizeof(lastSql));
    strncpy(lastSql, zsql, sizeof(lastSql) - 1);

    sqlite3_stmt* stmt = nullptr;
    const char* pzTail;
    int ret;
    if (ret = sqlite3_prepare_v3(_db, zsql, (int)strlen(zsql), 0, &stmt, &pzTail))
    {
        LOG_ERROR << "[sqlite3] sql \"" << zsql << "\" prepare error: [" << ret << "] " << errmsg();
        return ret;
    }

    int i = 1;
    for (auto& a : args)
    {
        if (a.type() == typeid(int)) sqlite3_bind_int(stmt, i, std::any_cast<int>(a));
        else if (a.type() == typeid(bool)) sqlite3_bind_int(stmt, i, int(std::any_cast<bool>(a)));
        else if (a.type() == typeid(long long)) sqlite3_bind_int64(stmt, i, std::any_cast<long long>(a));
        else if (a.type() == typeid(time_t)) sqlite3_bind_int64(stmt, i, std::any_cast<time_t>(a));
        else if (a.type() == typeid(double)) sqlite3_bind_double(stmt, i, std::any_cast<double>(a));
        else if (a.type() == typeid(std::string)) sqlite3_bind_text(stmt, i, std::any_cast<std::string>(a).c_str(), (int)std::any_cast<std::string>(a).length(), SQLITE_TRANSIENT);
        else if (a.type() == typeid(const char*)) sqlite3_bind_text(stmt, i, std::any_cast<const char*>(a), (int)strlen(std::any_cast<const char*>(a)), SQLITE_TRANSIENT);
        else if (a.type() == typeid(nullptr)) sqlite3_bind_null(stmt, i);
        else
        {
            LOG_ERROR << "[sqlite3] " << tag << ": " << " arg " << i << " type invalid " << errmsg();
            sqlite3_finalize(stmt);
            return -1;
        }
        ++i;
    }

    if ((ret = sqlite3_step(stmt)) != SQLITE_OK && ret != SQLITE_ROW && ret != SQLITE_DONE)
    {
        LOG_ERROR << "[sqlite3] " << tag << ": " << " exec " << zsql << ": " << errmsg();
        sqlite3_finalize(stmt);
        return ret;
    }
    LOG_DEBUG << "[sqlite3] " << tag << ": " << " exec " << zsql;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

void SQLite::transactionStart()
{
    if (!inTransaction)
        inTransaction = true;
    else
        return;

    sqlite3_stmt* stmt = nullptr;
    const char* pzTail;
    if (int ret = sqlite3_prepare_v3(_db, "BEGIN", 6, 0, &stmt, &pzTail))
        return;
    int ret;
    if ((ret = sqlite3_step(stmt)) != SQLITE_OK && ret != SQLITE_ROW && ret != SQLITE_DONE)
    {
        LOG_ERROR << "[sqlite3] " << tag << ": " << "Transaction start failed with error: " << errmsg();
    }
    else
    {
        LOG_ERROR << "[sqlite3] " << tag << ": " << "Transaction started";
    }
    sqlite3_finalize(stmt);
}

void SQLite::transactionStop()
{
    if (inTransaction)
        inTransaction = false;
    else
        return;

    sqlite3_stmt* stmt = nullptr;
    const char* pzTail;
    if (int ret = sqlite3_prepare_v3(_db, "COMMIT", 6, 0, &stmt, &pzTail))
        return;
    int ret;
    if ((ret = sqlite3_step(stmt)) != SQLITE_OK && ret != SQLITE_ROW && ret != SQLITE_DONE)
    {
        LOG_ERROR << "[sqlite3] " << tag << ": " << "Transaction end failed with error: " << errmsg();
    }
    else
    {
        LOG_ERROR << "[sqlite3] " << tag << ": " << "Transaction finished";
    }
    sqlite3_finalize(stmt);
}

void SQLite::commit()
{
    if (!inTransaction) exec("COMMIT");
    else LOG_WARNING << "[sqlite3] called Commit during transaction. Please call transactionStop";
}