#pragma once
#include "scene.h"
#include "scene_context.h"

class ScenePreSelect: public vScene
{
public:
    ScenePreSelect();
    virtual ~ScenePreSelect();

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    std::function<void()> _updateCallback;

    void updateLoadSongs();
    void updateLoadTables();

protected:
    SongListProperties rootFolderProp;
    bool startedLoadSong = false;
    bool startedLoadTable = false;
    std::future<void> loadSongEnd;
    std::future<void> loadTableEnd;
    int prevChartLoaded = 0;
    std::string textHint;
    std::string textHint2;
    bool loadingFinished = false;

public:
    bool isLoadingFinished() const;
};