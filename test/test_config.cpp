#include "gmock/gmock.h"
#include "config/config.h"
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

class tConfig : public vConfig
{
public:
    tConfig() : vConfig("dummy.cfg"_p) {}
    tConfig(StringPath path) : vConfig(path) {}

    virtual void setDefaults() noexcept
    {
        using namespace std::string_literals;
        set("ENTRY1", 1);
        set("ENTRY2", 2);
        set("ENTRY3", 3);
        set("ENTRY4", 4.0);
        set("ENTRY10", 10);
        set("ENTRYA", "A");
    }
};

TEST(Config, file_create)
{
    auto file = "test_create.cfg"_p;
    {
        tConfig c(file);
        c.setDefaults();
        c.save();
    }
    
    ASSERT_TRUE(fs::exists(file));
    fs::remove(file);
}

TEST(Config, modify)
{
    auto file = "test_modify.cfg"_p;
    {
        using namespace std::string_literals;
        tConfig c(file);
        c.setDefaults();
        c.set("ENTRY3", 9);
        c.set("ENTRYA", "TEST");
        c.save();
    }
    
    ASSERT_TRUE(fs::exists(file));

    {
        using namespace std::string_literals;
        tConfig c(file);
        c.load();
        EXPECT_EQ(c.get("ENTRY3", 3), 9);
        EXPECT_STREQ(c.get("ENTRYA", "A").c_str(), "TEST");
    }

    fs::remove(file);
}

TEST(Config, add)
{
    auto file = "test_add.cfg"_p;
    {
        tConfig c(file);
        c.setDefaults();
        c.set("ENTRY5", 5);
        c.save();
    }
    
    ASSERT_TRUE(fs::exists(file));

    {
        tConfig c(file);
        c.load();
        int entry5 = c.get("ENTRY5", 0);
        EXPECT_EQ(entry5, 5);
    }

    fs::remove(file);
}

TEST(Config, get_type_mismatch)
{
    auto file = "test.cfg"_p;
    {
        using namespace std::string_literals;
        tConfig c(file);
        c.setDefaults();
        EXPECT_EQ(c.get("ENTRY1", 4.0), 1.0);       // double can convert
        EXPECT_EQ(c.get("ENTRY1", 'e'), '1');       // char can convert
        EXPECT_EQ(c.get("ENTRY10", 'e'), 'e');      // string can not convert to char
        EXPECT_STREQ(c.get("ENTRY1", "ASDF").c_str(), "1");  // int can convert to string
    }
    if (fs::exists(file)) fs::remove(file);
}

TEST(Config, set_type_override)
{
    auto file = "test.cfg"_p;
    {
        using namespace std::string_literals;
        tConfig c(file);
        c.setDefaults();
        c.set("ENTRY1", "ASDF");
        EXPECT_EQ(c.get("ENTRY1", 50), 50);
        EXPECT_STREQ(c.get("ENTRY1", "A").c_str(), "ASDF");
    }
    if (fs::exists(file)) fs::remove(file);
}
