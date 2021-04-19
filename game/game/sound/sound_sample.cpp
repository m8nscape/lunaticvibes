#include "sound_sample.h"
#include "sound_mgr.h"
#include "config/config_mgr.h"

void loadLR2Bgm()
{
    auto pathstr = ConfigMgr::get("S", cfg::S_PATH_BGM, Path(cfg::S_DEFAULT_PATH_BGM).generic_string());
    Path path{ pathstr };

    //SoundMgr::loadSample((path / "").string(), static_cast<size_t>(eSoundSample::BGM_TITLE), true);
    //SoundMgr::loadSample((path / "").string(), static_cast<size_t>(eSoundSample::BGM_MODE), true);
    SoundMgr::loadSample((path / "select.wav").string(), static_cast<size_t>(eSoundSample::BGM_SELECT), true);
    SoundMgr::loadSample((path / "decide.wav").string(), static_cast<size_t>(eSoundSample::BGM_DECIDE), false);
}

void loadLR2Sound()
{
    auto pathstr = ConfigMgr::get("S", cfg::S_PATH_SOUND, Path(cfg::S_DEFAULT_PATH_SOUND).generic_string());
    Path path{ pathstr };

    SoundMgr::loadSample((path / "scratch.wav").string(), static_cast<size_t>(eSoundSample::SOUND_SCRATCH), false);
    SoundMgr::loadSample((path / "o-open.wav").string(), static_cast<size_t>(eSoundSample::SOUND_O_OPEN), false);
    SoundMgr::loadSample((path / "o-close.wav").string(), static_cast<size_t>(eSoundSample::SOUND_O_CLOSE), false);
    SoundMgr::loadSample((path / "o-change.wav").string(), static_cast<size_t>(eSoundSample::SOUND_O_CHANGE), false);
    SoundMgr::loadSample((path / "f-open.wav").string(), static_cast<size_t>(eSoundSample::SOUND_F_OPEN), false);
    SoundMgr::loadSample((path / "f-close.wav").string(), static_cast<size_t>(eSoundSample::SOUND_F_CLOSE), false);
    SoundMgr::loadSample((path / "difficulty.wav").string(), static_cast<size_t>(eSoundSample::SOUND_DIFFICULTY), false);

    SoundMgr::loadSample((path / "decide.wav").string(), static_cast<size_t>(eSoundSample::SOUND_DECIDE), false);
    SoundMgr::loadSample((path / "playstop.wav").string(), static_cast<size_t>(eSoundSample::SOUND_PLAYSTOP), false);
    SoundMgr::loadSample((path / "clear.wav").string(), static_cast<size_t>(eSoundSample::SOUND_CLEAR), false);
    SoundMgr::loadSample((path / "fail.wav").string(), static_cast<size_t>(eSoundSample::SOUND_FAIL), false);

    SoundMgr::loadSample((path / "screenshot.wav").string(), static_cast<size_t>(eSoundSample::SOUND_SCREENSHOT), false);

    SoundMgr::loadSample((path / "course_clear.wav").string(), static_cast<size_t>(eSoundSample::SOUND_COURSE_CLEAR), false);
    SoundMgr::loadSample((path / "course_fail.wav").string(), static_cast<size_t>(eSoundSample::SOUND_COURSE_FAIL), false);
}