#pragma once
#include <string>
#include <vector>
#include <memory>
#include "db_conn.h"
#include "common/types.h"
#include "common/utils.h"
#include "common/entry/entry_folder.h"
#include "common/entry/entry_song.h"

inline const HashMD5 ROOT_FOLDER_HASH = md5("", 0);

/* TABLE folder:
    md5(TEXT), parent(TEXT), path(TEXT), name(TEXT), type(INTEGER), removed(INTEGER)
    md5: hash string, calculated by "relative path to exe" OR "absolute path"
    parent: parent dir md5
    path: "relative path to exe" OR "absolute path"
    name: typically folder name, use path if NULL
    type: see enum FolderType
    removed: whether this folder has been removed
*/
/* TABLE song:
    md5(TEXT), folder(TEXT), type(INTEGER), file(TEXT), ...
    md5: file hash
    folder: folder hash
    type: see enum eChartFormat in chartformat.h
    file: file name (not including path)
*/
class SongDB: public SQLite
{
public:
    enum FolderType
    {
        FOLDER,
        SONG_BMS,

        CUSTOM_FOLDER,
        COURSE,
        CLASS,
    };

public:
    SongDB() = delete;
    SongDB(const char* path);
    SongDB(const Path& path) : SongDB(path.u8string().c_str()) {}
    ~SongDB();
    SongDB(SongDB&) = delete;
    SongDB& operator= (SongDB&) = delete;

protected:
    bool addChart(const HashMD5& folder, const Path& path);
    bool removeChart(const Path& path, const HashMD5& parent);
    bool removeChart(const HashMD5& md5, const HashMD5& parent);
    
public:
    std::vector<std::shared_ptr<ChartFormatBase>> findChartByName(const HashMD5& folder, const std::string&, unsigned limit = 1000) const;  // search from genre, version, artist, artist2, title, title2
    std::vector<std::shared_ptr<ChartFormatBase>> findChartByHash(const HashMD5&, bool checksum = true) const;  // chart may duplicate, return a list
    std::vector<std::shared_ptr<ChartFormatBase>> findChartFromTime(const HashMD5& folder, unsigned long long addTime) const;

protected:
    std::vector<std::vector<std::any>> songQueryPool;
    std::unordered_map<HashMD5, std::vector<size_t>> songQueryHashMap;
    std::unordered_map<HashMD5, std::vector<size_t>> songQueryParentMap;
    std::vector<std::vector<std::any>> folderQueryPool;
    std::unordered_map<HashMD5, std::vector<size_t>> folderQueryHashMap;
    std::unordered_map<HashMD5, std::vector<size_t>> folderQueryParentMap;
public:
    void prepareCache();
    void freeCache();

public:
    int initializeFolders(const std::vector<Path>& paths);
    int addSubFolder(Path path, const HashMD5& parent = ROOT_FOLDER_HASH);
    void waitLoadingFinish();
    int removeFolder(const HashMD5& hash, bool removeSong = false);

protected:
    int addNewFolder(const HashMD5& hash, const Path& path, const HashMD5& parent);
    int refreshExistingFolder(const HashMD5& hash, const Path& path, FolderType type);

public:
    HashMD5 getFolderParent(const HashMD5& folder) const;
    HashMD5 getFolderParent(const Path& path) const;
    std::pair<bool, Path> getFolderPath(const HashMD5& folder) const;
    HashMD5 getFolderHash(Path path) const;

    std::shared_ptr<EntryFolderRegular> browse(HashMD5 root, bool recursive = true);
    std::shared_ptr<EntryFolderSong> browseSong(HashMD5 root);
    std::shared_ptr<EntryFolderRegular> search(HashMD5 root, std::string key);

private:
    void* threadPool = nullptr;
    int poolThreadCount = 4;

public:
    int addChartTaskCount = 0;
    int addChartTaskFinishCount = 0;
    int addChartSuccess = 0;
    int addChartModified = 0;
    int addChartDeleted = 0;

    std::shared_mutex addCurrentPathMutex;
    std::string addCurrentPath;
    void resetAddSummary();

    bool stopRequested = false;
    void stopLoading();
};
