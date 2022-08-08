#include "sound_sample.h"
#include "sound_mgr.h"
#include "config/config_mgr.h"

void loadLR2Bgm()
{
    // TODO .lr2ss support

    Path path = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."),
        ConfigMgr::get("S", cfg::S_PATH_BGM, cfg::S_DEFAULT_PATH_BGM)));

    //SoundMgr::loadSysSample((path / ""), eSoundSample::BGM_TITLE, true);
    //SoundMgr::loadSysSample((path / ""), eSoundSample::BGM_MODE, true);
    SoundMgr::loadSysSample((path / "select.wav"), eSoundSample::BGM_SELECT, true, true);
    SoundMgr::loadSysSample((path / "decide.wav"), eSoundSample::BGM_DECIDE, false);
}

void loadLR2Sound()
{
    Path path = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."),
        ConfigMgr::get("S", cfg::S_PATH_SOUND, cfg::S_DEFAULT_PATH_SOUND)));

    SoundMgr::loadSysSample((path / "scratch.wav"), eSoundSample::SOUND_SCRATCH, false);
    SoundMgr::loadSysSample((path / "o-open.wav"), eSoundSample::SOUND_O_OPEN, false);
    SoundMgr::loadSysSample((path / "o-close.wav"), eSoundSample::SOUND_O_CLOSE, false);
    SoundMgr::loadSysSample((path / "o-change.wav"), eSoundSample::SOUND_O_CHANGE, false);
    SoundMgr::loadSysSample((path / "f-open.wav"), eSoundSample::SOUND_F_OPEN, false);
    SoundMgr::loadSysSample((path / "f-close.wav"), eSoundSample::SOUND_F_CLOSE, false);
    SoundMgr::loadSysSample((path / "difficulty.wav"), eSoundSample::SOUND_DIFFICULTY, false);

    SoundMgr::loadSysSample((path / "decide.wav"), eSoundSample::SOUND_DECIDE, false);
    SoundMgr::loadSysSample((path / "playstop.wav"), eSoundSample::SOUND_PLAYSTOP, false);
    SoundMgr::loadSysSample((path / "clear.wav"), eSoundSample::SOUND_CLEAR, false);
    SoundMgr::loadSysSample((path / "fail.wav"), eSoundSample::SOUND_FAIL, false);

    SoundMgr::loadSysSample((path / "screenshot.wav"), eSoundSample::SOUND_SCREENSHOT, false);

    SoundMgr::loadSysSample((path / "course_clear.wav"), eSoundSample::SOUND_COURSE_CLEAR, false);
    SoundMgr::loadSysSample((path / "course_fail.wav"), eSoundSample::SOUND_COURSE_FAIL, false);
}