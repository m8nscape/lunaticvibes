#include "setupdialog.h"
#include "ui_setupdialog.h"

#include "common/meta.h"
#include "config/config_mgr.h"

#include <filesystem>

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    fmodsystem = NULL;
    FMOD::System_Create(&fmodsystem);

    ui->setupUi(this);

    // set title
    QString title;

    // set version
    QString version;

    // get profile list
    refreshProfileList();

    // set profile
    std::string profile = ConfigMgr::get('E', cfg::E_PROFILE, PROFILE_DEFAULT);
    if (auto it = std::find(profileList.begin(), profileList.end(), profile); it != profileList.end())
    {
        loadProfile(profile);

        currentProfile = std::distance(profileList.begin(), it);
    }

    // set lr2 path
    lr2path = ConfigMgr::get('E', cfg::E_LR2PATH, ".");
}

SetupDialog::~SetupDialog()
{
    saveProfile();

    delete ui;

    fmodsystem->release();
}


void SetupDialog::setWindowMode(SetupDialog::WindowMode wm)
{
}
SetupDialog::WindowMode SetupDialog::getWindowMode()
{
}

void SetupDialog::setResolution(SetupDialog::Resolution res)
{
}
SetupDialog::Resolution SetupDialog::getResolution()
{
}


void SetupDialog::refreshProfileList()
{

}

bool SetupDialog::loadProfile(size_t idx)
{
    if (!std::filesystem::is_folder("profile/"s + profileList[idx])) return false;

    ConfigMgr::selectProfile(profileList[idx]);

    /////////////////////////////////////////////////////////////
    // Video
    {
        // get resolution
        int y = ConfigMgr::get('V', cfg::V_RES_X, 720);
        Resolution res = Resolution::HD_720p;
        switch (y)
        {
        case 640:  res = Resolution::SD_480p; break;
        case 720:  res = Resolution::HD_720p; break;
        case 1080: res = Resolution::FHD_1080p; break;
        default: break;
        }

        // get window mode
        auto wms = ConfigMgr::get('V', cfg::V_WINMODE, cfg::V_WINMODE_WINDOWED);
        WindowMode wm = WindowMode::WINDOWED;
        if      (wms == cfg::V_V_WINMODE_WINDOWED) wm = WindowMode::WINDOWED;
        else if (wms == cfg::V_V_WINMODE_FULL)     wm = WindowMode::FULL;
        else if (wms == cfg::BORDERLESS)           wm = WindowMode::BORDERLESS;

        // get vsync
        int vsync = ConfigMgr::get('V', cfg::V_VSYNC, 0);
    }


    /////////////////////////////////////////////////////////////
    // Audio
    {
        // set backend
        refreshAudioDeviceList();
        auto adev = ConfigMgr::get('A', cfg::A_DEVNAME, "");
        auto amod = ConfigMgr::get('A', cfg::A_MODE, "");
        if (auto it = std::find(audioDeviceList.begin(), audioDeviceList.end(), { adev, amod }}); it != audioDeviceList.end())
        {
            //currentAudioDevice = std::distance(audioDeviceList.begin(), it);
        }

        // set buffer count / size
        int audioBufferCount = ConfigMgr::get('A', cfg::A_BUFCOUNT, 4);
        int audioBufferSize = ConfigMgr::get('A', cfg::A_BUFSIZE, 128);
    }


    /////////////////////////////////////////////////////////////
    // Folders
    {
        
    }


    /////////////////////////////////////////////////////////////
    // Advanced
    {

    }


    return true;
}


bool SetupDialog::saveProfile()
{
    // check if selecting a valid profile
    
    // fetch from widget, save

    ConfigMgr::save();
    return true;
}

void SetupDialog::refreshAudioDeviceList()
{
    audioDeviceList.clear();
    if (!fmodsystem) return;

    int numDrivers = 0;
    char name[512];
    FMOD_GUID guid;
    int systemrate;
    FMOD_SPEAKERMODE speakermode;
    int speakermodechannels;

    // Auto
    fmodsystem->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
    fmodsystem->getNumDrivers(&numDrivers);
    for (int i = 0; i < numDrivers; ++i)
    {
        if (FMOD_OK == fmodsystem->getDriverInfo(i, name, sizeof(name), &guid, &systemrate, &speakermode, &speakermodechannels))
        {
            audioDeviceList.push_back({ name, cfg::A_MODE_AUTO });
        }
    }

#ifdef WIN32
    // ASIO
    fmodsystem->setOutput(FMOD_OUTPUTTYPE_ASIO);
    fmodsystem->getNumDrivers(&numDrivers);
    for (int i = 0; i < numDrivers; ++i)
    {
        if (FMOD_OK == fmodsystem->getDriverInfo(i, name, sizeof(name), &guid, &systemrate, &speakermode, &speakermodechannels))
        {
            audioDeviceList.push_back({ name, cfg::A_MODE_ASIO });
        }
    }
#endif

}