#pragma once

#include <string>

#include "utils.h"

template <size_t _Len>
class Hash
{
private:
    unsigned char data[_Len] = { 0 };
    bool set = false;
public:
    Hash() = default;
    Hash(const std::string& hex)
    {
        reset();
        if (!hex.empty())
        {
            set = true;
            std::string bin = hex2bin(hex);
            unsigned char* ubin = (unsigned char*)bin.data();
            for (int i = 0; i < bin.size() && i < _Len; ++i)
                data[i] = ubin[i];
        }
    }
    Hash(const Hash<_Len>& rhs)
    {
        reset();
        if (!rhs.empty())
        {
            set = true;
            for (int i = 0; i < _Len; ++i) data[i] = rhs.data[i];
        }
    }

    constexpr size_t length() const { return _Len; }
    bool empty() const { return !set; }
    std::string hexdigest() const { return bin2hex(data, _Len); }
    const unsigned char* hex() const { return data; }
    void reset() { set = false; memset(data, 0, _Len); }

    template <size_t _Len2>
    bool operator<(const Hash<_Len2>& rhs) const { return memcmp(data, rhs.data, _Len) < 0; }
    template <size_t _Len2>
    bool operator>(const Hash<_Len2>& rhs) const { return memcmp(data, rhs.data, _Len) > 0; }
    template <size_t _Len2>
    bool operator<=(const Hash<_Len2>& rhs) const { return !(*this > rhs); }
    template <size_t _Len2>
    bool operator>=(const Hash<_Len2>& rhs) const { return !(*this > rhs); }
    template <size_t _Len2>
    bool operator==(const Hash<_Len2>& rhs) const { return _Len == _Len2 && memcmp(data, rhs.data, _Len) == 0; }
    template <size_t _Len2>
    bool operator!=(const Hash<_Len2>& rhs) const { return _Len != _Len2 || memcmp(data, rhs.data, _Len) != 0; }

    friend struct std::hash<Hash<_Len>>;
};

template<size_t _Len>
struct std::hash<Hash<_Len>>
{
    size_t operator()(const Hash<_Len>& obj) const
    {
        size_t h = 0;
        int i = 0;
        if (_Len >= sizeof(size_t))
        {
            for (; i <= _Len - sizeof(size_t); i += sizeof(size_t))
            {
                h ^= *(size_t*)&obj.data[i];
            }
        }
        unsigned char* p = (unsigned char*)&h;
        for (; i < _Len; i++)
        {
            p[i % sizeof(size_t)] ^= obj.data[i];
        }
        return h;
    }
};

typedef Hash<16> HashMD5;
typedef Hash<32> HashSHA1;

HashMD5 md5(const std::string& str);
HashMD5 md5(const char* str, size_t len);
HashMD5 md5file(const Path& filePath);
