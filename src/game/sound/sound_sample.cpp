#include "sound_sample.h"
#include "sound_mgr.h"
#include "config/config_mgr.h"

void loadLR2Bgm()
{
    Path path = convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), 
        ConfigMgr::get("S", cfg::S_PATH_BGM, cfg::S_DEFAULT_PATH_BGM));

    //SoundMgr::loadSample((path / ""), eSoundSample::BGM_TITLE, true);
    //SoundMgr::loadSample((path / ""), eSoundSample::BGM_MODE, true);
    SoundMgr::loadSample((path / "select.wav"), eSoundSample::BGM_SELECT, true, true);
    SoundMgr::loadSample((path / "decide.wav"), eSoundSample::BGM_DECIDE, false);
}

void loadLR2Sound()
{
    Path path = convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), 
        ConfigMgr::get("S", cfg::S_PATH_SOUND, cfg::S_DEFAULT_PATH_SOUND));

    SoundMgr::loadSample((path / "scratch.wav"), eSoundSample::SOUND_SCRATCH, false);
    SoundMgr::loadSample((path / "o-open.wav"), eSoundSample::SOUND_O_OPEN, false);
    SoundMgr::loadSample((path / "o-close.wav"), eSoundSample::SOUND_O_CLOSE, false);
    SoundMgr::loadSample((path / "o-change.wav"), eSoundSample::SOUND_O_CHANGE, false);
    SoundMgr::loadSample((path / "f-open.wav"), eSoundSample::SOUND_F_OPEN, false);
    SoundMgr::loadSample((path / "f-close.wav"), eSoundSample::SOUND_F_CLOSE, false);
    SoundMgr::loadSample((path / "difficulty.wav"), eSoundSample::SOUND_DIFFICULTY, false);

    SoundMgr::loadSample((path / "decide.wav"), eSoundSample::SOUND_DECIDE, false);
    SoundMgr::loadSample((path / "playstop.wav"), eSoundSample::SOUND_PLAYSTOP, false);
    SoundMgr::loadSample((path / "clear.wav"), eSoundSample::SOUND_CLEAR, false, true);
    SoundMgr::loadSample((path / "fail.wav"), eSoundSample::SOUND_FAIL, false, true);

    SoundMgr::loadSample((path / "screenshot.wav"), eSoundSample::SOUND_SCREENSHOT, false);

    SoundMgr::loadSample((path / "course_clear.wav"), eSoundSample::SOUND_COURSE_CLEAR, false, true);
    SoundMgr::loadSample((path / "course_fail.wav"), eSoundSample::SOUND_COURSE_FAIL, false, true);
}