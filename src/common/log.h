#pragma once
#include <plog/Log.h>
#include <string>
#include <sstream>

#ifdef _WIN32
class RecordUTF8 : public plog::Record
{
public:
    RecordUTF8(plog::Severity severity, const char* func, size_t line, const char* file, const void* object, int instanceId)
        : plog::Record(severity, func, line, file, object, instanceId)
    {
    }

    RecordUTF8& ref()
    {
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    // Stream output operators

    RecordUTF8& operator<<(char data)
    {
        char str[] = { data, 0 };
        return *this << str;
    }

#if PLOG_ENABLE_WCHAR_INPUT
    RecordUTF8& operator<<(wchar_t data)
    {
        wchar_t str[] = { data, 0 };
        return *this << str;
    }

    RecordUTF8& operator<<(const wchar_t* data)
    {
        DWORD dwNum;
        dwNum = WideCharToMultiByte(CP_UTF8, NULL, data, -1, NULL, 0, NULL, FALSE);
        char* ustr = new char[dwNum];
        WideCharToMultiByte(CP_UTF8, NULL, data, -1, ustr, dwNum, NULL, FALSE);

        m_messageUTF8 << ustr;

        delete[] ustr;
        return *this;
    }

    RecordUTF8& operator<<(const std::wstring& data)
    {
        const wchar_t* wstr = data.c_str();
        return *this << wstr;
    }
#endif

    RecordUTF8& operator<<(std::wostream& (*data)(std::wostream&))
    {
        m_messageUTF8 << data;
        return *this;
    }

#ifdef QT_VERSION
    RecordUTF8& operator<<(const QString& data)
    {
#   ifdef _WIN32
        return *this << data.toStdWString();
#   else
        return *this << data.toStdString();
#   endif
    }

    RecordUTF8& operator<<(const QStringRef& data)
    {
        QString qstr;
        return *this << qstr.append(data);
    }
#endif

    template<typename T>
    RecordUTF8& operator<<(const T& data)
    {
        using namespace plog::detail;

        try
        {
            m_messageUTF8 << data;
        }
        catch (...)
        {
            m_messageUTF8 << " (* String codepage convert failed *) ";
        }
        return *this;
    }

    virtual const plog::util::nchar* getMessage() const override
    {
        DWORD dwNum;
        std::string messageStr = m_messageUTF8.str();
        dwNum = MultiByteToWideChar(CP_UTF8, 0, messageStr.c_str(), -1, NULL, 0);
        if (dwNum > 0)
        {
            m_messageStrUTF8.resize(dwNum);
            m_messageStrUTF8[dwNum - 1] = 0;
            MultiByteToWideChar(CP_UTF8, 0, messageStr.c_str(), -1, &m_messageStrUTF8[0], dwNum);
        }
        else
        {
            m_messageStrUTF8.clear();
        }
        return m_messageStrUTF8.c_str();
    }

private:
    std::stringstream    m_messageUTF8;
    mutable plog::util::nstring   m_messageStrUTF8;
};

#undef PLOG_
#define PLOG_(instanceId, severity)      IF_PLOG_(instanceId, severity) (*plog::get<instanceId>()) += RecordUTF8(severity, PLOG_GET_FUNC(), __LINE__, PLOG_GET_FILE(), PLOG_GET_THIS(), instanceId).ref()
#endif


int InitLogger();

// 0:Debug 1:Info 2:Warning 3:Error 4:Off?
void SetLogLevel(int level);

int FreeLogger();
