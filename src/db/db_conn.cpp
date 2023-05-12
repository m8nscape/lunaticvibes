#include "common/pch.h"

#include "sqlite3.h"
#include "db_conn.h"

namespace lunaticvibes
{

SQLite::SQLite(const char* path, const char* tag) : tag(tag)
{
    sqlite3_open(path, &_db);

    exec("PRAGMA temp_store = memory");
    exec("PRAGMA mmap_size = 536870912"); // 512MB
}

SQLite::~SQLite() { sqlite3_close(_db); }
const char* SQLite::errmsg() const { return sqlite3_errmsg(_db); }

std::string any_to_str(const std::any& a)
{
    std::stringstream ss;
    if (a.type() == typeid(int))              ss << std::any_cast<int>(a);
    else if (a.type() == typeid(bool))        ss << std::any_cast<bool>(a);
    else if (a.type() == typeid(long long))   ss << std::any_cast<long long>(a);
    else if (a.type() == typeid(time_t))      ss << std::any_cast<time_t>(a);
    else if (a.type() == typeid(double))      ss << std::any_cast<double>(a);
    else if (a.type() == typeid(std::string)) ss << "'" << std::any_cast<std::string>(a) << "'";
    else if (a.type() == typeid(const char*)) ss << "'" << std::any_cast<const char*>(a) << "'";
    else if (a.type() == typeid(nullptr))     ss << "NULL";
    return ss.str();
}

void sql_bind_any(sqlite3_stmt* stmt, int i, const std::any& a)
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
}
void sql_bind_any(sqlite3_stmt* stmt, const std::initializer_list<std::any>& args)
{
    int i = 1;
    for (auto& a : args)
    {
        sql_bind_any(stmt, i++, a);
    }
}

std::vector<std::vector<std::any>> SQLite::query(const char* zsql, size_t retSize, std::initializer_list<std::any> args) const
{
    _lastSql = zsql;

    size_t argc = args.size();

    sqlite3_stmt* stmt = nullptr;
    const char* pzTail;
    if (int ret = sqlite3_prepare_v3(_db, zsql, (int)strlen(zsql), 0, &stmt, &pzTail))
    {
        LOG_ERROR << "[sqlite3] sql \"" << zsql << "\" prepare error: [" << ret << "] " << errmsg();
        return {};
    }
    sql_bind_any(stmt, args);

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

#if _DEBUG
    std::stringstream ss;
    ss << "[sqlite3] " << tag << ": " << " query " << zsql;
    ss << " (args: ";
    for (auto& a : args)
    {
        ss << any_to_str(a) << ", ";
    }
    ss << ") result: " << ret.size() << " rows";
    LOG_DEBUG << ss.str();
#endif

    sqlite3_finalize(stmt);
    return ret;
}

int SQLite::exec(const char* zsql, std::initializer_list<std::any> args)
{
    _lastSql = zsql;

    sqlite3_stmt* stmt = nullptr;
    const char* pzTail;
    int ret;
    if (ret = sqlite3_prepare_v3(_db, zsql, (int)strlen(zsql), 0, &stmt, &pzTail))
    {
        LOG_ERROR << "[sqlite3] sql \"" << zsql << "\" prepare error: [" << ret << "] " << errmsg();
        return ret;
    }

    sql_bind_any(stmt, args);

    ret = sqlite3_step(stmt);

    if (ret != SQLITE_OK && ret != SQLITE_ROW && ret != SQLITE_DONE)
    {
        LOG_ERROR << "[sqlite3] " << tag << ": " << " exec " << zsql << ": " << errmsg();
        sqlite3_finalize(stmt);
        return ret;
    }

#if _DEBUG
    std::stringstream ss;
    ss << "[sqlite3] " << tag << ": " << " exec " << zsql;
    ss << " (args: ";
    for (auto& a : args)
    {
        ss << any_to_str(a) << ", ";
    }
    ss << ")";
    if (ret != SQLITE_OK && ret != SQLITE_ROW && ret != SQLITE_DONE)
        LOG_ERROR << ss.str() << ": " << errmsg();
    else
        LOG_DEBUG << ss.str();
#endif

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
        LOG_DEBUG << "[sqlite3] " << tag << ": " << "Transaction start";
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
        LOG_DEBUG << "[sqlite3] " << tag << ": " << "Transaction finished";
    }
    sqlite3_finalize(stmt);
}

void SQLite::optimize()
{
    LOG_DEBUG << "[sqlite3] " << tag << ": optimize ";
    exec("PRAGMA optimize(0xfffe)");
}

void SQLite::commit()
{
    LOG_DEBUG << "[sqlite3] " << tag << ": commit";
    if (!inTransaction) exec("COMMIT");
    else LOG_WARNING << "[sqlite3] called Commit during transaction. Please call transactionStop";
}

}
