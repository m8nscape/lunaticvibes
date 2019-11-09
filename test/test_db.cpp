#include "gmock/gmock.h"
#include "db/db_song.h"

const StringPath pathSongDB = "test_song.db"_p;

TEST(SongDB, init)
{
    std::filesystem::current_path("F:\\banshao_test");
    SongDB db{ Path(pathSongDB) };
    ASSERT_TRUE(std::filesystem::exists(pathSongDB));
}

TEST(SongDB, table_folder)
{
    std::filesystem::current_path("F:\\banshao_test");
    SongDB db{ Path(pathSongDB) };
    ASSERT_TRUE(std::filesystem::exists(pathSongDB));
    auto a = std::filesystem::absolute("res/test").string();
    ASSERT_TRUE(std::filesystem::exists(std::filesystem::absolute("res/test").string()));
    ASSERT_EQ(SQLITE_OK, db.addFolder(std::filesystem::absolute("res/test").string()));
    ASSERT_TRUE(std::filesystem::exists("res/test"));
    ASSERT_EQ(SQLITE_OK, db.addFolder("res/test"));
}

TEST(SongDB, table_song)
{
    std::filesystem::current_path("F:\\banshao_test");
    SongDB db{ Path(pathSongDB) };
    ASSERT_TRUE(std::filesystem::exists(pathSongDB));
    ASSERT_TRUE(std::filesystem::exists("res/test"));

    auto hash = db.getFolderHash("res/test/db_song");
    auto list = db.browse(hash);
    ASSERT_FALSE(list.empty());

    ASSERT_EQ(4, list.getContentsCount());
}

TEST(SongDB, remove_table)
{
    std::filesystem::current_path("F:\\banshao_test");
    SongDB db{ Path(pathSongDB) };
    auto hash = db.getFolderHash("res/test");
    ASSERT_FALSE(hash.empty());
    ASSERT_EQ(0, db.removeFolder(hash, true));

    auto hash1 = db.getFolderHash(std::filesystem::absolute("res/test").string());
    ASSERT_FALSE(hash1.empty());
    ASSERT_EQ(0, db.removeFolder(hash1, true));
}
