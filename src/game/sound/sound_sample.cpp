#include "common/pch.h"
#include "sound_sample.h"
#include "sound_mgr.h"
#include "config/config_mgr.h"
#include "soundset_lr2.h"

void loadLR2Sound()
{
    LOG_INFO << "[Sound] Load system samples from LR2 Soundset";

    Path path = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."),
        ConfigMgr::get("S", cfg::S_PATH_SOUNDSET, cfg::S_DEFAULT_PATH_SOUNDSET)));

    SoundSetLR2 ss(path);

    //SoundMgr::loadSysSample((path / ""), eSoundSample::BGM_TITLE, true);
    //SoundMgr::loadSysSample((path / ""), eSoundSample::BGM_MODE, true);
    SoundMgr::loadSysSample(ss.getPathBGMSelect(), eSoundSample::BGM_SELECT, true, true);
    SoundMgr::loadSysSample(ss.getPathBGMDecide(), eSoundSample::BGM_DECIDE, false);

    SoundMgr::loadSysSample(ss.getPathSoundScratch(), eSoundSample::SOUND_SCRATCH, false);
    SoundMgr::loadSysSample(ss.getPathSoundOpenPanel(), eSoundSample::SOUND_O_OPEN, false);
    SoundMgr::loadSysSample(ss.getPathSoundClosePanel(), eSoundSample::SOUND_O_CLOSE, false);
    SoundMgr::loadSysSample(ss.getPathSoundOptionChange(), eSoundSample::SOUND_O_CHANGE, false);
    SoundMgr::loadSysSample(ss.getPathSoundOpenFolder(), eSoundSample::SOUND_F_OPEN, false);
    SoundMgr::loadSysSample(ss.getPathSoundCloseFolder(), eSoundSample::SOUND_F_CLOSE, false);
    SoundMgr::loadSysSample(ss.getPathSoundDifficultyChange(), eSoundSample::SOUND_DIFFICULTY, false);

    SoundMgr::loadSysSample(ss.getPathBGMDecide(), eSoundSample::SOUND_DECIDE, false);
    SoundMgr::loadSysSample(ss.getPathSoundFailed(), eSoundSample::SOUND_PLAYSTOP, false);
    SoundMgr::loadSysSample(ss.getPathBGMResultClear(), eSoundSample::SOUND_CLEAR, false);
    SoundMgr::loadSysSample(ss.getPathBGMResultFailed(), eSoundSample::SOUND_FAIL, false);

    SoundMgr::loadSysSample(ss.getPathSoundLandmine(), eSoundSample::SOUND_LANDMINE, false);

    SoundMgr::loadSysSample(ss.getPathSoundScreenshot(), eSoundSample::SOUND_SCREENSHOT, false);

    SoundMgr::loadSysSample(ss.getPathBGMCourseClear(), eSoundSample::SOUND_COURSE_CLEAR, false);
    SoundMgr::loadSysSample(ss.getPathBGMCourseFailed(), eSoundSample::SOUND_COURSE_FAIL, false);
}